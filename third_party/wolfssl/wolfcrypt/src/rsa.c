/* rsa.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifndef NO_RSA

#if defined(HAVE_FIPS) && \
    defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2)

    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS

       #ifdef USE_WINDOWS_API
               #pragma code_seg(".fipsA$e")
               #pragma const_seg(".fipsB$e")
       #endif
#endif

#include <wolfssl/wolfcrypt/rsa.h>

#ifdef WOLFSSL_HAVE_SP_RSA
#include <wolfssl/wolfcrypt/sp.h>
#endif

/*
Possible RSA enable options:
 * NO_RSA:              Overall control of RSA                      default: on (not defined)
 * WC_RSA_BLINDING:     Uses Blinding w/ Private Ops                default: off
                        Note: slower by ~20%
 * WOLFSSL_KEY_GEN:     Allows Private Key Generation               default: off
 * RSA_LOW_MEM:         NON CRT Private Operations, less memory     default: off
 * WC_NO_RSA_OAEP:      Disables RSA OAEP padding                   default: on (not defined)

*/

/*
RSA Key Size Configuration:
 * FP_MAX_BITS:         With USE_FAST_MATH only                     default: 4096
    If USE_FAST_MATH then use this to override default.
    Value is key size * 2. Example: RSA 3072 = 6144
*/


/* If building for old FIPS. */
#if defined(HAVE_FIPS) && \
    (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION < 2))

int  wc_InitRsaKey(RsaKey* key, void* ptr)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    return InitRsaKey_fips(key, ptr);
}


int  wc_InitRsaKey_ex(RsaKey* key, void* ptr, int devId)
{
    (void)devId;
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    return InitRsaKey_fips(key, ptr);
}


int  wc_FreeRsaKey(RsaKey* key)
{
    return FreeRsaKey_fips(key);
}


int  wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out,
                                 word32 outLen, RsaKey* key, WC_RNG* rng)
{
    if (in == NULL || out == NULL || key == NULL || rng == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPublicEncrypt_fips(in, inLen, out, outLen, key, rng);
}


int  wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out,
                                        RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPrivateDecryptInline_fips(in, inLen, out, key);
}


int  wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                  word32 outLen, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaPrivateDecrypt_fips(in, inLen, out, outLen, key);
}


int  wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, WC_RNG* rng)
{
    if (in == NULL || out == NULL || key == NULL || inLen == 0) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_Sign_fips(in, inLen, out, outLen, key, rng);
}


int  wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_VerifyInline_fips(in, inLen, out, key);
}


int  wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out,
                              word32 outLen, RsaKey* key)
{
    if (in == NULL || out == NULL || key == NULL || inLen == 0) {
        return BAD_FUNC_ARG;
    }
    return RsaSSL_Verify_fips(in, inLen, out, outLen, key);
}


int  wc_RsaEncryptSize(RsaKey* key)
{
    if (key == NULL) {
        return BAD_FUNC_ARG;
    }
    return RsaEncryptSize_fips(key);
}


int wc_RsaFlattenPublicKey(RsaKey* key, byte* a, word32* aSz, byte* b,
                           word32* bSz)
{

    /* not specified as fips so not needing _fips */
    return RsaFlattenPublicKey(key, a, aSz, b, bSz);
}


#ifdef WOLFSSL_KEY_GEN
    int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
    {
        return MakeRsaKey(key, size, e, rng);
    }
#endif


/* these are functions in asn and are routed to wolfssl/wolfcrypt/asn.c
* wc_RsaPrivateKeyDecode
* wc_RsaPublicKeyDecode
*/

#else /* else build without fips, or for new fips */

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifdef WOLF_CRYPTO_DEV
    #include <wolfssl/wolfcrypt/cryptodev.h>
#endif
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#define ERROR_OUT(x) { ret = (x); goto done;}


enum {
    RSA_STATE_NONE = 0,

    RSA_STATE_ENCRYPT_PAD,
    RSA_STATE_ENCRYPT_EXPTMOD,
    RSA_STATE_ENCRYPT_RES,

    RSA_STATE_DECRYPT_EXPTMOD,
    RSA_STATE_DECRYPT_UNPAD,
    RSA_STATE_DECRYPT_RES,
};

static void wc_RsaCleanup(RsaKey* key)
{
    if (key && key->data) {
        /* make sure any allocated memory is free'd */
        if (key->dataIsAlloc) {
            if (key->type == RSA_PRIVATE_DECRYPT ||
                key->type == RSA_PRIVATE_ENCRYPT) {
                ForceZero(key->data, key->dataLen);
            }
            XFREE(key->data, key->heap, DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 0;
        }
        key->data = NULL;
        key->dataLen = 0;
    }
}

int wc_InitRsaKey_ex(RsaKey* key, void* heap, int devId)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(key, 0, sizeof(RsaKey));

    key->type = RSA_TYPE_UNKNOWN;
    key->state = RSA_STATE_NONE;
    key->heap = heap;
    key->data = NULL;
    key->dataLen = 0;
    key->dataIsAlloc = 0;
#ifdef WC_RSA_BLINDING
    key->rng = NULL;
#endif

#ifdef WOLF_CRYPTO_DEV
    key->devId = devId;
#else
    (void)devId;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #ifdef WOLFSSL_CERT_GEN
        XMEMSET(&key->certSignCtx, 0, sizeof(CertSignCtx));
    #endif

    #ifdef WC_ASYNC_ENABLE_RSA
        /* handle as async */
        ret = wolfAsync_DevCtxInit(&key->asyncDev, WOLFSSL_ASYNC_MARKER_RSA,
                                                            key->heap, devId);
        if (ret != 0)
            return ret;
    #endif /* WC_ASYNC_ENABLE_RSA */
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = mp_init_multi(&key->n, &key->e, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY)
        return ret;

    ret = mp_init_multi(&key->d, &key->p, &key->q, &key->dP, &key->dQ, &key->u);
    if (ret != MP_OKAY) {
        mp_clear(&key->n);
        mp_clear(&key->e);
        return ret;
    }

#ifdef WOLFSSL_XILINX_CRYPT
    key->pubExp = 0;
    key->mod    = NULL;
#endif

    return ret;
}

int wc_InitRsaKey(RsaKey* key, void* heap)
{
    return wc_InitRsaKey_ex(key, heap, INVALID_DEVID);
}


#ifdef WOLFSSL_XILINX_CRYPT
#define MAX_E_SIZE 4
/* Used to setup hardware state
 *
 * key  the RSA key to setup
 *
 * returns 0 on success
 */
int wc_InitRsaHw(RsaKey* key)
{
    unsigned char* m; /* RSA modulous */
    word32 e = 0;     /* RSA public exponent */
    int mSz;
    int eSz;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    mSz = mp_unsigned_bin_size(&(key->n));
    m = (unsigned char*)XMALLOC(mSz, key->heap, DYNAMIC_TYPE_KEY);
    if (m == 0) {
        return MEMORY_E;
    }

    if (mp_to_unsigned_bin(&(key->n), m) != MP_OKAY) {
        WOLFSSL_MSG("Unable to get RSA key modulus");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return MP_READ_E;
    }

    eSz = mp_unsigned_bin_size(&(key->e));
    if (eSz > MAX_E_SIZE) {
        WOLFSSL_MSG("Exponent of size 4 bytes expected");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_FUNC_ARG;
    }

    if (mp_to_unsigned_bin(&(key->e), (byte*)&e + (MAX_E_SIZE - eSz))
                != MP_OKAY) {
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        WOLFSSL_MSG("Unable to get RSA key exponent");
        return MP_READ_E;
    }

    /* check for existing mod buffer to avoid memory leak */
    if (key->mod != NULL) {
        XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    }

    key->pubExp = e;
    key->mod    = m;

    if (XSecure_RsaInitialize(&(key->xRsa), key->mod, NULL,
                (byte*)&(key->pubExp)) != XST_SUCCESS) {
        WOLFSSL_MSG("Unable to initialize RSA on hardware");
        XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
        return BAD_STATE_E;
    }

#ifdef WOLFSSL_XILINX_PATCH
   /* currently a patch of xsecure_rsa.c for 2048 bit keys */
   if (wc_RsaEncryptSize(key) == 256) {
       if (XSecure_RsaSetSize(&(key->xRsa), 2048) != XST_SUCCESS) {
           WOLFSSL_MSG("Unable to set RSA key size on hardware");
           XFREE(m, key->heap, DYNAMIC_TYPE_KEY);
           return BAD_STATE_E;
       }
   }
#endif

    return 0;
}
#endif /* WOLFSSL_XILINX_CRYPT */

int wc_FreeRsaKey(RsaKey* key)
{
    int ret = 0;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    wc_RsaCleanup(key);

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    wolfAsync_DevCtxFree(&key->asyncDev, WOLFSSL_ASYNC_MARKER_RSA);
#endif

    if (key->type == RSA_PRIVATE) {
        mp_forcezero(&key->u);
        mp_forcezero(&key->dQ);
        mp_forcezero(&key->dP);
        mp_forcezero(&key->q);
        mp_forcezero(&key->p);
        mp_forcezero(&key->d);
    }
    /* private part */
    mp_clear(&key->u);
    mp_clear(&key->dQ);
    mp_clear(&key->dP);
    mp_clear(&key->q);
    mp_clear(&key->p);
    mp_clear(&key->d);

    /* public part */
    mp_clear(&key->e);
    mp_clear(&key->n);

#ifdef WOLFSSL_XILINX_CRYPT
    XFREE(key->mod, key->heap, DYNAMIC_TYPE_KEY);
    key->mod = NULL;
#endif

    return ret;
}


#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_PSS)
/* Uses MGF1 standard as a mask generation function
   hType: hash type used
   seed:  seed to use for generating mask
   seedSz: size of seed buffer
   out:   mask output after generation
   outSz: size of output buffer
 */
static int RsaMGF1(enum wc_HashType hType, byte* seed, word32 seedSz,
                                        byte* out, word32 outSz, void* heap)
{
    byte* tmp;
    /* needs to be large enough for seed size plus counter(4) */
    byte  tmpA[WC_MAX_DIGEST_SIZE + 4];
    byte   tmpF;     /* 1 if dynamic memory needs freed */
    word32 tmpSz;
    int hLen;
    int ret;
    word32 counter;
    word32 idx;
    hLen    = wc_HashGetDigestSize(hType);
    counter = 0;
    idx     = 0;

    (void)heap;

    /* check error return of wc_HashGetDigestSize */
    if (hLen < 0) {
        return hLen;
    }

    /* if tmp is not large enough than use some dynamic memory */
    if ((seedSz + 4) > sizeof(tmpA) || (word32)hLen > sizeof(tmpA)) {
        /* find largest amount of memory needed which will be the max of
         * hLen and (seedSz + 4) since tmp is used to store the hash digest */
        tmpSz = ((seedSz + 4) > (word32)hLen)? seedSz + 4: (word32)hLen;
        tmp = (byte*)XMALLOC(tmpSz, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (tmp == NULL) {
            return MEMORY_E;
        }
        tmpF = 1; /* make sure to free memory when done */
    }
    else {
        /* use array on the stack */
        tmpSz = sizeof(tmpA);
        tmp  = tmpA;
        tmpF = 0; /* no need to free memory at end */
    }

    do {
        int i = 0;
        XMEMCPY(tmp, seed, seedSz);

        /* counter to byte array appended to tmp */
        tmp[seedSz]     = (counter >> 24) & 0xFF;
        tmp[seedSz + 1] = (counter >> 16) & 0xFF;
        tmp[seedSz + 2] = (counter >>  8) & 0xFF;
        tmp[seedSz + 3] = (counter)       & 0xFF;

        /* hash and append to existing output */
        if ((ret = wc_Hash(hType, tmp, (seedSz + 4), tmp, tmpSz)) != 0) {
            /* check for if dynamic memory was needed, then free */
            if (tmpF) {
                XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            }
            return ret;
        }

        for (i = 0; i < hLen && idx < outSz; i++) {
            out[idx++] = tmp[i];
        }
        counter++;
    } while (idx < outSz);

    /* check for if dynamic memory was needed, then free */
    if (tmpF) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
    }

    return 0;
}

/* helper function to direct which mask generation function is used
   switeched on type input
 */
static int RsaMGF(int type, byte* seed, word32 seedSz, byte* out,
                                                    word32 outSz, void* heap)
{
    int ret;

    switch(type) {
    #ifndef NO_SHA
        case WC_MGF1SHA1:
            ret = RsaMGF1(WC_HASH_TYPE_SHA, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifndef NO_SHA256
    #ifdef WOLFSSL_SHA224
        case WC_MGF1SHA224:
            ret = RsaMGF1(WC_HASH_TYPE_SHA224, seed, seedSz, out, outSz, heap);
            break;
    #endif
        case WC_MGF1SHA256:
            ret = RsaMGF1(WC_HASH_TYPE_SHA256, seed, seedSz, out, outSz, heap);
            break;
    #endif
    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case WC_MGF1SHA384:
            ret = RsaMGF1(WC_HASH_TYPE_SHA384, seed, seedSz, out, outSz, heap);
            break;
    #endif
        case WC_MGF1SHA512:
            ret = RsaMGF1(WC_HASH_TYPE_SHA512, seed, seedSz, out, outSz, heap);
            break;
    #endif
        default:
            WOLFSSL_MSG("Unknown MGF type: check build options");
            ret = BAD_FUNC_ARG;
    }

    /* in case of default avoid unused warning */
    (void)seed;
    (void)seedSz;
    (void)out;
    (void)outSz;
    (void)heap;

    return ret;
}
#endif /* !WC_NO_RSA_OAEP */


/* Padding */
#ifndef WC_NO_RSA_OAEP
static int RsaPad_OAEP(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, byte padValue, WC_RNG* rng,
        enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
        void* heap)
{
    int ret;
    int hLen;
    int psLen;
    int i;
    word32 idx;

    byte* dbMask;

    #ifdef WOLFSSL_SMALL_STACK
        byte* lHash = NULL;
        byte* seed  = NULL;
    #else
        /* must be large enough to contain largest hash */
        byte lHash[WC_MAX_DIGEST_SIZE];
        byte seed[ WC_MAX_DIGEST_SIZE];
    #endif

    /* no label is allowed, but catch if no label provided and length > 0 */
    if (optLabel == NULL && labelLen > 0) {
        return BUFFER_E;
    }

    /* limit of label is the same as limit of hash function which is massive */
    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0) {
        return hLen;
    }

    #ifdef WOLFSSL_SMALL_STACK
        lHash = (byte*)XMALLOC(hLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (lHash == NULL) {
            return MEMORY_E;
        }
        seed = (byte*)XMALLOC(hLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
        if (seed == NULL) {
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            return MEMORY_E;
        }
    #else
        /* hLen should never be larger than lHash since size is max digest size,
           but check before blindly calling wc_Hash */
        if ((word32)hLen > sizeof(lHash)) {
            WOLFSSL_MSG("OAEP lHash to small for digest!!");
            return MEMORY_E;
        }
    #endif

    if ((ret = wc_Hash(hType, optLabel, labelLen, lHash, hLen)) != 0) {
        WOLFSSL_MSG("OAEP hash type possibly not supported or lHash to small");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    /* handles check of location for idx as well as psLen, cast to int to check
       for pkcsBlockLen(k) - 2 * hLen - 2 being negative
       This check is similar to decryption where k > 2 * hLen + 2 as msg
       size approaches 0. In decryption if k is less than or equal -- then there
       is no possible room for msg.
       k = RSA key size
       hLen = hash digest size -- will always be >= 0 at this point
     */
    if ((word32)(2 * hLen + 2) > pkcsBlockLen) {
        WOLFSSL_MSG("OAEP pad error hash to big for RSA key size");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BAD_FUNC_ARG;
    }

    if (inputLen > (pkcsBlockLen - 2 * hLen - 2)) {
        WOLFSSL_MSG("OAEP pad error message too long");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BAD_FUNC_ARG;
    }

    /* concatenate lHash || PS || 0x01 || msg */
    idx = pkcsBlockLen - 1 - inputLen;
    psLen = pkcsBlockLen - inputLen - 2 * hLen - 2;
    if (pkcsBlockLen < inputLen) { /*make sure not writing over end of buffer */
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return BUFFER_E;
    }
    XMEMCPY(pkcsBlock + (pkcsBlockLen - inputLen), input, inputLen);
    pkcsBlock[idx--] = 0x01; /* PS and M separator */
    while (psLen > 0 && idx > 0) {
        pkcsBlock[idx--] = 0x00;
        psLen--;
    }

    idx = idx - hLen + 1;
    XMEMCPY(pkcsBlock + idx, lHash, hLen);

    /* generate random seed */
    if ((ret = wc_RNG_GenerateBlock(rng, seed, hLen)) != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    /* create maskedDB from dbMask */
    dbMask = (byte*)XMALLOC(pkcsBlockLen - hLen - 1, heap, DYNAMIC_TYPE_RSA);
    if (dbMask == NULL) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return MEMORY_E;
    }
    XMEMSET(dbMask, 0, pkcsBlockLen - hLen - 1); /* help static analyzer */

    ret = RsaMGF(mgf, seed, hLen, dbMask, pkcsBlockLen - hLen - 1, heap);
    if (ret != 0) {
        XFREE(dbMask, heap, DYNAMIC_TYPE_RSA);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    i = 0;
    idx = hLen + 1;
    while (idx < pkcsBlockLen && (word32)i < (pkcsBlockLen - hLen -1)) {
        pkcsBlock[idx] = dbMask[i++] ^ pkcsBlock[idx];
        idx++;
    }
    XFREE(dbMask, heap, DYNAMIC_TYPE_RSA);


    /* create maskedSeed from seedMask */
    idx = 0;
    pkcsBlock[idx++] = 0x00;
    /* create seedMask inline */
    if ((ret = RsaMGF(mgf, pkcsBlock + hLen + 1, pkcsBlockLen - hLen - 1,
                                           pkcsBlock + 1, hLen, heap)) != 0) {
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
            XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
        #endif
        return ret;
    }

    /* xor created seedMask with seed to make maskedSeed */
    i = 0;
    while (idx < (word32)(hLen + 1) && i < hLen) {
        pkcsBlock[idx] = pkcsBlock[idx] ^ seed[i++];
        idx++;
    }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(lHash, heap, DYNAMIC_TYPE_RSA_BUFFER);
        XFREE(seed,  heap, DYNAMIC_TYPE_RSA_BUFFER);
    #endif
    (void)padValue;

    return 0;
}
#endif /* !WC_NO_RSA_OAEP */

#ifdef WC_RSA_PSS

/* 0x00 .. 0x00 0x01 | Salt | Gen Hash | 0xbc
 * XOR MGF over all bytes down to end of Salt
 * Gen Hash = HASH(8 * 0x00 | Message Hash | Salt)
 *
 * input         Digest of the message.
 * inputLen      Length of digest.
 * pkcsBlock     Buffer to write to.
 * pkcsBlockLen  Length of buffer to write to.
 * rng           Random number generator (for salt).
 * htype         Hash function to use.
 * mgf           Mask generation function.
 * saltLen       Length of salt to put in padding.
 * bits          Length of key in bits.
 * heap          Used for dynamic memory allocation.
 * returns 0 on success, PSS_SALTLEN_E when the salt length is invalid
 * and other negative values on error.
 */
static int RsaPad_PSS(const byte* input, word32 inputLen, byte* pkcsBlock,
        word32 pkcsBlockLen, WC_RNG* rng, enum wc_HashType hType, int mgf,
        int saltLen, int bits, void* heap)
{
    int   ret;
    int   hLen, i;
    byte* s;
    byte* m;
    byte* h;
    byte  salt[WC_MAX_DIGEST_SIZE];

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;

    if (saltLen == -1)
        saltLen = hLen;
    else if (saltLen > hLen || saltLen < -1)
        return PSS_SALTLEN_E;
    if ((int)pkcsBlockLen - hLen - 1 < saltLen + 2)
        return PSS_SALTLEN_E;

    s = m = pkcsBlock;
    XMEMSET(m, 0, RSA_PSS_PAD_SZ);
    m += RSA_PSS_PAD_SZ;
    XMEMCPY(m, input, inputLen);
    m += inputLen;
    if ((ret = wc_RNG_GenerateBlock(rng, salt, saltLen)) != 0)
        return ret;
    XMEMCPY(m, salt, saltLen);
    m += saltLen;

    h = pkcsBlock + pkcsBlockLen - 1 - hLen;
    if ((ret = wc_Hash(hType, s, (word32)(m - s), h, hLen)) != 0)
        return ret;
    pkcsBlock[pkcsBlockLen - 1] = RSA_PSS_PAD_TERM;

    ret = RsaMGF(mgf, h, hLen, pkcsBlock, pkcsBlockLen - hLen - 1, heap);
    if (ret != 0)
        return ret;
    pkcsBlock[0] &= (1 << ((bits - 1) & 0x7)) - 1;

    m = pkcsBlock + pkcsBlockLen - 1 - saltLen - hLen - 1;
    *(m++) ^= 0x01;
    for (i = 0; i < saltLen; i++)
        m[i] ^= salt[i];

    return 0;
}
#endif

static int RsaPad(const byte* input, word32 inputLen, byte* pkcsBlock,
                           word32 pkcsBlockLen, byte padValue, WC_RNG* rng)
{
    if (input == NULL || inputLen == 0 || pkcsBlock == NULL ||
                                                        pkcsBlockLen == 0) {
        return BAD_FUNC_ARG;
    }

    pkcsBlock[0] = 0x0;       /* set first byte to zero and advance */
    pkcsBlock++; pkcsBlockLen--;
    pkcsBlock[0] = padValue;  /* insert padValue */

    if (padValue == RSA_BLOCK_TYPE_1) {
        if (pkcsBlockLen < inputLen + 2) {
            WOLFSSL_MSG("RsaPad error, invalid length");
            return RSA_PAD_E;
        }

        /* pad with 0xff bytes */
        XMEMSET(&pkcsBlock[1], 0xFF, pkcsBlockLen - inputLen - 2);
    }
    else {
        /* pad with non-zero random bytes */
        word32 padLen, i;
        int    ret;

        if (pkcsBlockLen < inputLen + 1) {
            WOLFSSL_MSG("RsaPad error, invalid length");
            return RSA_PAD_E;
        }

        padLen = pkcsBlockLen - inputLen - 1;
        ret    = wc_RNG_GenerateBlock(rng, &pkcsBlock[1], padLen);
        if (ret != 0) {
            return ret;
        }

        /* remove zeros */
        for (i = 1; i < padLen; i++) {
            if (pkcsBlock[i] == 0) pkcsBlock[i] = 0x01;
        }
    }

    pkcsBlock[pkcsBlockLen-inputLen-1] = 0;     /* separator */
    XMEMCPY(pkcsBlock+pkcsBlockLen-inputLen, input, inputLen);

    return 0;
}

/* helper function to direct which padding is used */
static int wc_RsaPad_ex(const byte* input, word32 inputLen, byte* pkcsBlock,
    word32 pkcsBlockLen, byte padValue, WC_RNG* rng, int padType,
    enum wc_HashType hType, int mgf, byte* optLabel, word32 labelLen,
    int saltLen, int bits, void* heap)
{
    int ret;

    switch (padType)
    {
        case WC_RSA_PKCSV15_PAD:
            /*WOLFSSL_MSG("wolfSSL Using RSA PKCSV15 padding");*/
            ret = RsaPad(input, inputLen, pkcsBlock, pkcsBlockLen,
                                                                 padValue, rng);
            break;

    #ifndef WC_NO_RSA_OAEP
        case WC_RSA_OAEP_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA OAEP padding");
            ret = RsaPad_OAEP(input, inputLen, pkcsBlock, pkcsBlockLen,
                           padValue, rng, hType, mgf, optLabel, labelLen, heap);
            break;
    #endif

    #ifdef WC_RSA_PSS
        case WC_RSA_PSS_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA PSS padding");
            ret = RsaPad_PSS(input, inputLen, pkcsBlock, pkcsBlockLen, rng,
                                               hType, mgf, saltLen, bits, heap);
            break;
    #endif

    #ifdef WC_RSA_NO_PADDING
        case WC_RSA_NO_PAD:
            WOLFSSL_MSG("wolfSSL Using NO padding");

            /* In the case of no padding being used check that input is exactly
             * the RSA key length */
            if (bits <= 0 || inputLen != ((word32)bits/WOLFSSL_BIT_SIZE)) {
                WOLFSSL_MSG("Bad input size");
                ret = RSA_PAD_E;
            }
            else {
                XMEMCPY(pkcsBlock, input, inputLen);
                ret = 0;
            }
            break;
    #endif

        default:
            WOLFSSL_MSG("Unknown RSA Pad Type");
            ret = RSA_PAD_E;
    }

    /* silence warning if not used with padding scheme */
    (void)hType;
    (void)mgf;
    (void)optLabel;
    (void)labelLen;
    (void)saltLen;
    (void)bits;
    (void)heap;

    return ret;
}


/* UnPadding */
#ifndef WC_NO_RSA_OAEP
/* UnPad plaintext, set start to *output, return length of plaintext,
 * < 0 on error */
static int RsaUnPad_OAEP(byte *pkcsBlock, unsigned int pkcsBlockLen,
                            byte **output, enum wc_HashType hType, int mgf,
                            byte* optLabel, word32 labelLen, void* heap)
{
    int hLen;
    int ret;
    byte h[WC_MAX_DIGEST_SIZE]; /* max digest size */
    byte* tmp;
    word32 idx;

    /* no label is allowed, but catch if no label provided and length > 0 */
    if (optLabel == NULL && labelLen > 0) {
        return BUFFER_E;
    }

    hLen = wc_HashGetDigestSize(hType);
    if ((hLen < 0) || (pkcsBlockLen < (2 * (word32)hLen + 2))) {
        return BAD_FUNC_ARG;
    }

    tmp = (byte*)XMALLOC(pkcsBlockLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL) {
        return MEMORY_E;
    }
    XMEMSET(tmp, 0, pkcsBlockLen);

    /* find seedMask value */
    if ((ret = RsaMGF(mgf, (byte*)(pkcsBlock + (hLen + 1)),
                            pkcsBlockLen - hLen - 1, tmp, hLen, heap)) != 0) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    /* xor seedMask value with maskedSeed to get seed value */
    for (idx = 0; idx < (word32)hLen; idx++) {
        tmp[idx] = tmp[idx] ^ pkcsBlock[1 + idx];
    }

    /* get dbMask value */
    if ((ret = RsaMGF(mgf, tmp, hLen, tmp + hLen,
                                       pkcsBlockLen - hLen - 1, heap)) != 0) {
        XFREE(tmp, NULL, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    /* get DB value by doing maskedDB xor dbMask */
    for (idx = 0; idx < (pkcsBlockLen - hLen - 1); idx++) {
        pkcsBlock[hLen + 1 + idx] = pkcsBlock[hLen + 1 + idx] ^ tmp[idx + hLen];
    }

    /* done with use of tmp buffer */
    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);

    /* advance idx to index of PS and msg separator, account for PS size of 0*/
    idx = hLen + 1 + hLen;
    while (idx < pkcsBlockLen && pkcsBlock[idx] == 0) {idx++;}

    /* create hash of label for comparison with hash sent */
    if ((ret = wc_Hash(hType, optLabel, labelLen, h, hLen)) != 0) {
        return ret;
    }

    /* say no to chosen ciphertext attack.
       Comparison of lHash, Y, and separator value needs to all happen in
       constant time.
       Attackers should not be able to get error condition from the timing of
       these checks.
     */
    ret = 0;
    ret |= ConstantCompare(pkcsBlock + hLen + 1, h, hLen);
    ret += pkcsBlock[idx++] ^ 0x01; /* separator value is 0x01 */
    ret += pkcsBlock[0]     ^ 0x00; /* Y, the first value, should be 0 */

    if (ret != 0) {
        WOLFSSL_MSG("RsaUnPad_OAEP: Padding Error");
        return BAD_PADDING_E;
    }

    /* adjust pointer to correct location in array and return size of M */
    *output = (byte*)(pkcsBlock + idx);
    return pkcsBlockLen - idx;
}
#endif /* WC_NO_RSA_OAEP */

#ifdef WC_RSA_PSS
/* 0x00 .. 0x00 0x01 | Salt | Gen Hash | 0xbc
 * MGF over all bytes down to end of Salt
 *
 * pkcsBlock     Buffer holding decrypted data.
 * pkcsBlockLen  Length of buffer.
 * htype         Hash function to use.
 * mgf           Mask generation function.
 * saltLen       Length of salt to put in padding.
 * bits          Length of key in bits.
 * heap          Used for dynamic memory allocation.
 * returns 0 on success, PSS_SALTLEN_E when the salt length is invalid,
 * BAD_PADDING_E when the padding is not valid, MEMORY_E when allocation fails
 * and other negative values on error.
 */
static int RsaUnPad_PSS(byte *pkcsBlock, unsigned int pkcsBlockLen,
                        byte **output, enum wc_HashType hType, int mgf,
                        int saltLen, int bits, void* heap)
{
    int   ret;
    byte* tmp;
    int   hLen, i;

    hLen = wc_HashGetDigestSize(hType);
    if (hLen < 0)
        return hLen;

    if (saltLen == -1)
        saltLen = hLen;
    else if (saltLen > hLen || saltLen < -1)
        return PSS_SALTLEN_E;
    if ((int)pkcsBlockLen - hLen - 1 < saltLen + 2)
        return PSS_SALTLEN_E;

    if (pkcsBlock[pkcsBlockLen - 1] != RSA_PSS_PAD_TERM) {
        WOLFSSL_MSG("RsaUnPad_PSS: Padding Term Error");
        return BAD_PADDING_E;
    }

    tmp = (byte*)XMALLOC(pkcsBlockLen, heap, DYNAMIC_TYPE_RSA_BUFFER);
    if (tmp == NULL)
        return MEMORY_E;

    if ((ret = RsaMGF(mgf, pkcsBlock + pkcsBlockLen - 1 - hLen, hLen,
                                    tmp, pkcsBlockLen - 1 - hLen, heap)) != 0) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        return ret;
    }

    tmp[0] &= (1 << ((bits - 1) & 0x7)) - 1;
    for (i = 0; i < (int)(pkcsBlockLen - 1 - saltLen - hLen - 1); i++) {
        if (tmp[i] != pkcsBlock[i]) {
            XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
            WOLFSSL_MSG("RsaUnPad_PSS: Padding Error Match");
            return BAD_PADDING_E;
        }
    }
    if (tmp[i] != (pkcsBlock[i] ^ 0x01)) {
        XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);
        WOLFSSL_MSG("RsaUnPad_PSS: Padding Error End");
        return BAD_PADDING_E;
    }
    for (i++; i < (int)(pkcsBlockLen - 1 - hLen); i++)
        pkcsBlock[i] ^= tmp[i];

    XFREE(tmp, heap, DYNAMIC_TYPE_RSA_BUFFER);

    i = pkcsBlockLen - (RSA_PSS_PAD_SZ + saltLen + 2 * hLen + 1);
    XMEMSET(pkcsBlock + i, 0, RSA_PSS_PAD_SZ);

    *output = pkcsBlock + i;
    return RSA_PSS_PAD_SZ + saltLen + 2 * hLen;
}
#endif

/* UnPad plaintext, set start to *output, return length of plaintext,
 * < 0 on error */
static int RsaUnPad(const byte *pkcsBlock, unsigned int pkcsBlockLen,
                                               byte **output, byte padValue)
{
    word32 maxOutputLen = (pkcsBlockLen > 10) ? (pkcsBlockLen - 10) : 0;
    word32 invalid = 0;
    word32 i = 1;
    word32 outputLen;

    if (output == NULL || pkcsBlockLen == 0) {
        return BAD_FUNC_ARG;
    }

    if (pkcsBlock[0] != 0x0) { /* skip past zero */
        invalid = 1;
    }
    pkcsBlock++; pkcsBlockLen--;

    /* Require block type padValue */
    invalid = (pkcsBlock[0] != padValue) || invalid;

    /* verify the padding until we find the separator */
    if (padValue == RSA_BLOCK_TYPE_1) {
        while (i<pkcsBlockLen && pkcsBlock[i++] == 0xFF) {/* Null body */}
    }
    else {
        while (i<pkcsBlockLen && pkcsBlock[i++]) {/* Null body */}
    }

    if (!(i==pkcsBlockLen || pkcsBlock[i-1]==0)) {
        WOLFSSL_MSG("RsaUnPad error, bad formatting");
        return RSA_PAD_E;
    }

    outputLen = pkcsBlockLen - i;
    invalid = (outputLen > maxOutputLen) || invalid;

    if (invalid) {
        WOLFSSL_MSG("RsaUnPad error, invalid formatting");
        return RSA_PAD_E;
    }

    *output = (byte *)(pkcsBlock + i);
    return outputLen;
}

/* helper function to direct unpadding
 *
 * bits is the key modulus size in bits
 */
static int wc_RsaUnPad_ex(byte* pkcsBlock, word32 pkcsBlockLen, byte** out,
                          byte padValue, int padType, enum wc_HashType hType,
                          int mgf, byte* optLabel, word32 labelLen, int saltLen,
                          int bits, void* heap)
{
    int ret;

    switch (padType) {
        case WC_RSA_PKCSV15_PAD:
            /*WOLFSSL_MSG("wolfSSL Using RSA PKCSV15 un-padding");*/
            ret = RsaUnPad(pkcsBlock, pkcsBlockLen, out, padValue);
            break;

    #ifndef WC_NO_RSA_OAEP
        case WC_RSA_OAEP_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA OAEP un-padding");
            ret = RsaUnPad_OAEP((byte*)pkcsBlock, pkcsBlockLen, out,
                                        hType, mgf, optLabel, labelLen, heap);
            break;
    #endif

    #ifdef WC_RSA_PSS
        case WC_RSA_PSS_PAD:
            WOLFSSL_MSG("wolfSSL Using RSA PSS un-padding");
            ret = RsaUnPad_PSS((byte*)pkcsBlock, pkcsBlockLen, out, hType, mgf,
                                                           saltLen, bits, heap);
            break;
    #endif

    #ifdef WC_RSA_NO_PADDING
        case WC_RSA_NO_PAD:
            WOLFSSL_MSG("wolfSSL Using NO un-padding");

            /* In the case of no padding being used check that input is exactly
             * the RSA key length */
            if (bits <= 0 || pkcsBlockLen != ((word32)bits/WOLFSSL_BIT_SIZE)) {
                WOLFSSL_MSG("Bad input size");
                ret = RSA_PAD_E;
            }
            else {
                if (out != NULL) {
                    *out = pkcsBlock;
                }
                ret = pkcsBlockLen;
            }
            break;
    #endif /* WC_RSA_NO_PADDING */

        default:
            WOLFSSL_MSG("Unknown RSA UnPad Type");
            ret = RSA_PAD_E;
    }

    /* silence warning if not used with padding scheme */
    (void)hType;
    (void)mgf;
    (void)optLabel;
    (void)labelLen;
    (void)saltLen;
    (void)bits;
    (void)heap;

    return ret;
}

#if defined(WOLFSSL_XILINX_CRYPT)
/*
 * Xilinx hardened crypto acceleration.
 *
 * Returns 0 on success and negative values on error.
 */
static int wc_RsaFunctionXil(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int    ret = 0;
    word32 keyLen, len;
    (void)rng;

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        WOLFSSL_MSG("Output buffer is not big enough");
        return BAD_FUNC_ARG;
    }

    if (inLen != keyLen) {
        WOLFSSL_MSG("Expected that inLen equals RSA key length");
        return BAD_FUNC_ARG;
    }

    switch(type) {
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
        /* Currently public exponent is loaded by default.
         * In SDK 2017.1 RSA exponent values are expected to be of 4 bytes
         * leading to private key operations with Xsecure_RsaDecrypt not being
         * supported */
        ret = RSA_WRONG_TYPE_E;
        break;
    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
        if (XSecure_RsaDecrypt(&(key->xRsa), in, out) != XST_SUCCESS) {
            ret = BAD_STATE_E;
        }
        break;
    default:
        ret = RSA_WRONG_TYPE_E;
    }

    *outLen = keyLen;

    return ret;
}
#endif /* WOLFSSL_XILINX_CRYPT */

static int wc_RsaFunctionSync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
#ifndef WOLFSSL_SP_MATH
    mp_int tmp;
#ifdef WC_RSA_BLINDING
    mp_int rnd, rndi;
#endif
    int    ret = 0;
    word32 keyLen, len;
#endif

#ifdef WOLFSSL_HAVE_SP_RSA
#ifndef WOLFSSL_SP_NO_2048
    if (mp_count_bits(&key->n) == 2048) {
        switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
            return sp_RsaPrivate_2048(in, inLen, &key->d, &key->p, &key->q,
                                      &key->dP, &key->dQ, &key->u, &key->n,
                                      out, outLen);
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_2048(in, inLen, &key->e, &key->n, out, outLen);
        }
    }
#endif
#ifndef WOLFSSL_SP_NO_3072
    if (mp_count_bits(&key->n) == 3072) {
        switch(type) {
        case RSA_PRIVATE_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
    #ifdef WC_RSA_BLINDING
            if (rng == NULL)
                return MISSING_RNG_E;
    #endif
            return sp_RsaPrivate_3072(in, inLen, &key->d, &key->p, &key->q,
                                      &key->dP, &key->dQ, &key->u, &key->n,
                                      out, outLen);
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
            return sp_RsaPublic_3072(in, inLen, &key->e, &key->n, out, outLen);
        }
    }
#endif
#endif /* WOLFSSL_HAVE_SP_RSA */

#ifdef WOLFSSL_SP_MATH
    return WC_KEY_SIZE_E;
#else
    (void)rng;

    if (mp_init(&tmp) != MP_OKAY)
        return MP_INIT_E;

#ifdef WC_RSA_BLINDING
    if (type == RSA_PRIVATE_DECRYPT || type == RSA_PRIVATE_ENCRYPT) {
        if (mp_init_multi(&rnd, &rndi, NULL, NULL, NULL, NULL) != MP_OKAY) {
            mp_clear(&tmp);
            return MP_INIT_E;
        }
    }
#endif

    if (mp_read_unsigned_bin(&tmp, (byte*)in, inLen) != MP_OKAY)
        ERROR_OUT(MP_READ_E);

    switch(type) {
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    {
    #ifdef WC_RSA_BLINDING
        /* blind */
        ret = mp_rand(&rnd, get_digit_count(&key->n), rng);
        if (ret != MP_OKAY)
            goto done;

        /* rndi = 1/rnd mod n */
        if (mp_invmod(&rnd, &key->n, &rndi) != MP_OKAY)
            ERROR_OUT(MP_INVMOD_E);

        /* rnd = rnd^e */
        if (mp_exptmod(&rnd, &key->e, &key->n, &rnd) != MP_OKAY)
            ERROR_OUT(MP_EXPTMOD_E);

        /* tmp = tmp*rnd mod n */
        if (mp_mulmod(&tmp, &rnd, &key->n, &tmp) != MP_OKAY)
            ERROR_OUT(MP_MULMOD_E);
    #endif /* WC_RSA_BLINDING */

    #ifdef RSA_LOW_MEM      /* half as much memory but twice as slow */
        if (mp_exptmod(&tmp, &key->d, &key->n, &tmp) != MP_OKAY)
            ERROR_OUT(MP_EXPTMOD_E);
    #else
        /* Return 0 when cond is false and n when cond is true. */
        #define COND_N(cond, n)    ((0 - (cond)) & (n))
        /* If ret has an error value return it otherwise if r is OK then return
         * 0 otherwise return e.
         */
        #define RET_ERR(ret, r, e) \
            ((ret) | (COND_N((ret) == 0, COND_N((r) != MP_OKAY, (e)))))

        { /* tmpa/b scope */
        mp_int tmpa, tmpb;
        int r;

        if (mp_init(&tmpa) != MP_OKAY)
            ERROR_OUT(MP_INIT_E);

        if (mp_init(&tmpb) != MP_OKAY) {
            mp_clear(&tmpa);
            ERROR_OUT(MP_INIT_E);
        }

        /* tmpa = tmp^dP mod p */
        r = mp_exptmod(&tmp, &key->dP, &key->p, &tmpa);
        ret = RET_ERR(ret, r, MP_EXPTMOD_E);

        /* tmpb = tmp^dQ mod q */
        r = mp_exptmod(&tmp, &key->dQ, &key->q, &tmpb);
        ret = RET_ERR(ret, r, MP_EXPTMOD_E);

        /* tmp = (tmpa - tmpb) * qInv (mod p) */
        r = mp_sub(&tmpa, &tmpb, &tmp);
        ret = RET_ERR(ret, r, MP_SUB_E);

        r = mp_mulmod(&tmp, &key->u, &key->p, &tmp);
        ret = RET_ERR(ret, r, MP_MULMOD_E);

        /* tmp = tmpb + q * tmp */
        r = mp_mul(&tmp, &key->q, &tmp);
        ret = RET_ERR(ret, r, MP_MUL_E);

        r = mp_add(&tmp, &tmpb, &tmp);
        ret = RET_ERR(ret, r, MP_ADD_E);

        mp_clear(&tmpa);
        mp_clear(&tmpb);

        if (ret != 0) {
            goto done;
        }
        #undef RET_ERR
        #undef COND_N
        } /* tmpa/b scope */
    #endif   /* RSA_LOW_MEM */

    #ifdef WC_RSA_BLINDING
        /* unblind */
        if (mp_mulmod(&tmp, &rndi, &key->n, &tmp) != MP_OKAY)
            ERROR_OUT(MP_MULMOD_E);
    #endif   /* WC_RSA_BLINDING */

        break;
    }
    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
    #ifdef WOLFSSL_XILINX_CRYPT
        ret = wc_RsaFunctionXil(in, inLen, out, outLen, type, key, rng);
        goto done;
    #else
        if (mp_exptmod(&tmp, &key->e, &key->n, &tmp) != MP_OKAY)
            ERROR_OUT(MP_EXPTMOD_E);
        break;
    #endif
    default:
        ERROR_OUT(RSA_WRONG_TYPE_E);
    }

    keyLen = wc_RsaEncryptSize(key);
    if (keyLen > *outLen) {
        ERROR_OUT(RSA_BUFFER_E);
    }

    len = mp_unsigned_bin_size(&tmp);

    /* pad front w/ zeros to match key length */
    while (len < keyLen) {
        *out++ = 0x00;
        len++;
    }

    *outLen = keyLen;

    /* convert */
    if (mp_to_unsigned_bin(&tmp, out) != MP_OKAY)
        ERROR_OUT(MP_TO_E);

done:
    mp_clear(&tmp);
#ifdef WC_RSA_BLINDING
    if (type == RSA_PRIVATE_DECRYPT || type == RSA_PRIVATE_ENCRYPT) {
        mp_clear(&rndi);
        mp_clear(&rnd);
    }
#endif
    return ret;
#endif
}

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
static int wc_RsaFunctionAsync(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = 0;

    (void)rng;

#ifdef WOLFSSL_ASYNC_CRYPT_TEST
    if (wc_AsyncTestInit(&key->asyncDev, ASYNC_TEST_RSA_FUNC)) {
        WC_ASYNC_TEST* testDev = &key->asyncDev.test;
        testDev->rsaFunc.in = in;
        testDev->rsaFunc.inSz = inLen;
        testDev->rsaFunc.out = out;
        testDev->rsaFunc.outSz = outLen;
        testDev->rsaFunc.type = type;
        testDev->rsaFunc.key = key;
        testDev->rsaFunc.rng = rng;
        return WC_PENDING_E;
    }
#endif /* WOLFSSL_ASYNC_CRYPT_TEST */

    switch(type) {
    case RSA_PRIVATE_DECRYPT:
    case RSA_PRIVATE_ENCRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->d.raw.buf, key->d.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        #ifdef RSA_LOW_MEM
            ret = IntelQaRsaPrivate(&key->asyncDev, in, inLen,
                                    &key->d.raw, &key->n.raw,
                                    out, outLen);
        #else
            ret = IntelQaRsaCrtPrivate(&key->asyncDev, in, inLen,
                                &key->p.raw, &key->q.raw,
                                &key->dP.raw, &key->dQ.raw,
                                &key->u.raw,
                                out, outLen);
        #endif
    #else /* WOLFSSL_ASYNC_CRYPT_TEST */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;

    case RSA_PUBLIC_ENCRYPT:
    case RSA_PUBLIC_DECRYPT:
    #ifdef HAVE_CAVIUM
        key->dataLen = key->n.raw.len;
        ret = NitroxRsaExptMod(in, inLen,
                               key->e.raw.buf, key->e.raw.len,
                               key->n.raw.buf, key->n.raw.len,
                               out, outLen, key);
    #elif defined(HAVE_INTEL_QA)
        ret = IntelQaRsaPublic(&key->asyncDev, in, inLen,
                               &key->e.raw, &key->n.raw,
                               out, outLen);
    #else /* WOLFSSL_ASYNC_CRYPT_TEST */
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    #endif
        break;

    default:
        ret = RSA_WRONG_TYPE_E;
    }

    return ret;
}
#endif /* WOLFSSL_ASYNC_CRYPT && WC_ASYNC_ENABLE_RSA */

#if defined(WC_RSA_DIRECT) || defined(WC_RSA_NO_PADDING)
/* Function that does the RSA operation directly with no padding.
 *
 * in       buffer to do operation on
 * inLen    length of input buffer
 * out      buffer to hold results
 * outSz    gets set to size of result buffer. Should be passed in as length
 *          of out buffer. If the pointer "out" is null then outSz gets set to
 *          the expected buffer size needed and LENGTH_ONLY_E gets returned.
 * key      RSA key to use for encrypt/decrypt
 * type     if using private or public key {RSA_PUBLIC_ENCRYPT,
 *          RSA_PUBLIC_DECRYPT, RSA_PRIVATE_ENCRYPT, RSA_PRIVATE_DECRYPT}
 * rng      wolfSSL RNG to use if needed
 *
 * returns size of result on success
 */
int wc_RsaDirect(byte* in, word32 inLen, byte* out, word32* outSz,
        RsaKey* key, int type, WC_RNG* rng)
{
    int ret;

    if (in == NULL || outSz == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check on type of RSA operation */
    switch (type) {
        case RSA_PUBLIC_ENCRYPT:
        case RSA_PUBLIC_DECRYPT:
        case RSA_PRIVATE_ENCRYPT:
        case RSA_PRIVATE_DECRYPT:
            break;
        default:
            WOLFSSL_MSG("Bad RSA type");
            return BAD_FUNC_ARG;
    }

    if ((ret = wc_RsaEncryptSize(key)) < 0) {
        return BAD_FUNC_ARG;
    }

    if (inLen != (word32)ret) {
        WOLFSSL_MSG("Bad input length. Should be RSA key size");
        return BAD_FUNC_ARG;
    }

    if (out == NULL) {
        *outSz = inLen;
        return LENGTH_ONLY_E;
    }

    switch (key->state) {
        case RSA_STATE_NONE:
        case RSA_STATE_ENCRYPT_PAD:
        case RSA_STATE_ENCRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_EXPTMOD:
        case RSA_STATE_DECRYPT_UNPAD:
            key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_EXPTMOD:
                                                  RSA_STATE_DECRYPT_EXPTMOD;

            key->dataLen = *outSz;

            ret = wc_RsaFunction(in, inLen, out, &key->dataLen, type, key, rng);
            if (ret >= 0 || ret == WC_PENDING_E) {
                key->state = (type == RSA_PRIVATE_ENCRYPT ||
                    type == RSA_PUBLIC_ENCRYPT) ? RSA_STATE_ENCRYPT_RES:
                                                  RSA_STATE_DECRYPT_RES;
            }
            if (ret < 0) {
                break;
            }

            FALL_THROUGH;

        case RSA_STATE_ENCRYPT_RES:
        case RSA_STATE_DECRYPT_RES:
            ret = key->dataLen;
            break;

        default:
            ret = BAD_STATE_E;
    }

    /* if async pending then skip cleanup*/
    if (ret == WC_PENDING_E) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}
#endif /* WC_RSA_DIRECT || WC_RSA_NO_PADDING */


int wc_RsaFunction(const byte* in, word32 inLen, byte* out,
                          word32* outLen, int type, RsaKey* key, WC_RNG* rng)
{
    int ret = 0;

    if (key == NULL || in == NULL || inLen == 0 || out == NULL ||
            outLen == NULL || *outLen == 0 || type == RSA_TYPE_UNKNOWN) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLF_CRYPTO_DEV
    if (key->devId != INVALID_DEVID) {
        ret = wc_CryptoDev_Rsa(in, inLen, out, outLen, type, key, rng);
        if (ret != NOT_COMPILED_IN)
            return ret;
        ret = 0; /* reset error code and try using software */
    }
#endif

#ifndef NO_RSA_BOUNDS_CHECK
    if (type == RSA_PRIVATE_DECRYPT &&
        key->state == RSA_STATE_DECRYPT_EXPTMOD) {

        /* Check that 1 < in < n-1. (Requirement of 800-56B.) */
        mp_int c;

        if (mp_init(&c) != MP_OKAY)
            ret = MEMORY_E;
        if (ret == 0) {
            if (mp_read_unsigned_bin(&c, in, inLen) != 0)
                ret = MP_READ_E;
        }
        if (ret == 0) {
            /* check c > 1 */
            if (mp_cmp_d(&c, 1) != MP_GT)
                ret = RSA_OUT_OF_RANGE_E;
        }
        if (ret == 0) {
            /* add c+1 */
            if (mp_add_d(&c, 1, &c) != MP_OKAY)
                ret = MP_ADD_E;
        }
        if (ret == 0) {
            /* check c+1 < n */
            if (mp_cmp(&c, &key->n) != MP_LT)
                ret = RSA_OUT_OF_RANGE_E;
        }
        mp_clear(&c);

        if (ret != 0)
            return ret;
    }
#endif /* NO_RSA_BOUNDS_CHECK */

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                        key->n.raw.len > 0) {
        ret = wc_RsaFunctionAsync(in, inLen, out, outLen, type, key, rng);
    }
    else
#endif
    {
        ret = wc_RsaFunctionSync(in, inLen, out, outLen, type, key, rng);
    }

    /* handle error */
    if (ret < 0 && ret != WC_PENDING_E) {
        if (ret == MP_EXPTMOD_E) {
            /* This can happen due to incorrectly set FP_MAX_BITS or missing XREALLOC */
            WOLFSSL_MSG("RSA_FUNCTION MP_EXPTMOD_E: memory/config problem");
        }

        key->state = RSA_STATE_NONE;
        wc_RsaCleanup(key);
    }

    return ret;
}


/* Internal Wrappers */
/* Gives the option of choosing padding type
   in : input to be encrypted
   inLen: length of input buffer
   out: encrypted output
   outLen: length of encrypted output buffer
   key   : wolfSSL initialized RSA key struct
   rng   : wolfSSL initialized random number struct
   rsa_type  : type of RSA: RSA_PUBLIC_ENCRYPT, RSA_PUBLIC_DECRYPT,
        RSA_PRIVATE_ENCRYPT or RSA_PRIVATE_DECRYPT
   pad_value: RSA_BLOCK_TYPE_1 or RSA_BLOCK_TYPE_2
   pad_type  : type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
        WC_RSA_NO_PAD or WC_RSA_PSS_PAD
   hash  : type of hash algorithm to use found in wolfssl/wolfcrypt/hash.h
   mgf   : type of mask generation function to use
   label : optional label
   labelSz : size of optional label buffer
   saltLen : Length of salt used in PSS
   rng : random number generator */
static int RsaPublicEncryptEx(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int rsa_type,
                            byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret, sz;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = wc_RsaEncryptSize(key);
    if (sz > (int)outLen) {
        return RSA_BUFFER_E;
    }

    if (sz < RSA_MIN_PAD_SZ) {
        return WC_KEY_SIZE_E;
    }

    if (inLen > (word32)(sz - RSA_MIN_PAD_SZ)) {
#ifdef WC_RSA_NO_PADDING
        /* In the case that no padding is used the input length can and should
         * be the same size as the RSA key. */
        if (pad_type != WC_RSA_NO_PAD)
#endif
        return RSA_BUFFER_E;
    }

    switch (key->state) {
    case RSA_STATE_NONE:
    case RSA_STATE_ENCRYPT_PAD:
        key->state = RSA_STATE_ENCRYPT_PAD;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                 pad_type != WC_RSA_PSS_PAD && key->n.raw.buf) {
            /* Async operations that include padding */
            if (rsa_type == RSA_PUBLIC_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaPublicEncrypt(in, inLen, out, outLen, key);
            }
            else if (rsa_type == RSA_PRIVATE_ENCRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_ENCRYPT_RES;
                key->dataLen = key->n.raw.len;
                return NitroxRsaSSL_Sign(in, inLen, out, outLen, key);
            }
        }
    #endif

        ret = wc_RsaPad_ex(in, inLen, out, sz, pad_value, rng, pad_type, hash,
                           mgf, label, labelSz, saltLen, mp_count_bits(&key->n),
                           key->heap);
        if (ret < 0) {
            break;
        }

        key->state = RSA_STATE_ENCRYPT_EXPTMOD;

        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_EXPTMOD:

        key->dataLen = outLen;
        ret = wc_RsaFunction(out, sz, out, &key->dataLen, rsa_type, key, rng);

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_ENCRYPT_RES;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_ENCRYPT_RES:
        ret = key->dataLen;
        break;

    default:
        ret = BAD_STATE_E;
        break;
    }

    /* if async pending then return and skip done cleanup below */
    if (ret == WC_PENDING_E) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}

/* Gives the option of choosing padding type
   in : input to be decrypted
   inLen: length of input buffer
   out:  decrypted message
   outLen: length of decrypted message in bytes
   outPtr: optional inline output pointer (if provided doing inline)
   key   : wolfSSL initialized RSA key struct
   rsa_type  : type of RSA: RSA_PUBLIC_ENCRYPT, RSA_PUBLIC_DECRYPT,
        RSA_PRIVATE_ENCRYPT or RSA_PRIVATE_DECRYPT
   pad_value: RSA_BLOCK_TYPE_1 or RSA_BLOCK_TYPE_2
   pad_type  : type of padding: WC_RSA_PKCSV15_PAD, WC_RSA_OAEP_PAD,
        WC_RSA_NO_PAD, WC_RSA_PSS_PAD
   hash  : type of hash algorithm to use found in wolfssl/wolfcrypt/hash.h
   mgf   : type of mask generation function to use
   label : optional label
   labelSz : size of optional label buffer
   saltLen : Length of salt used in PSS
   rng : random number generator */
static int RsaPrivateDecryptEx(byte* in, word32 inLen, byte* out,
                            word32 outLen, byte** outPtr, RsaKey* key,
                            int rsa_type, byte pad_value, int pad_type,
                            enum wc_HashType hash, int mgf,
                            byte* label, word32 labelSz, int saltLen,
                            WC_RNG* rng)
{
    int ret = RSA_WRONG_TYPE_E;

    if (in == NULL || inLen == 0 || out == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    switch (key->state) {
    case RSA_STATE_NONE:
    case RSA_STATE_DECRYPT_EXPTMOD:
        key->state = RSA_STATE_DECRYPT_EXPTMOD;
        key->dataLen = inLen;

    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        /* Async operations that include padding */
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
            if (rsa_type == RSA_PRIVATE_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_2) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaPrivateDecrypt(in, inLen, out, &key->dataLen,
                                               key);
            }
            else if (rsa_type == RSA_PUBLIC_DECRYPT &&
                                                pad_value == RSA_BLOCK_TYPE_1) {
                key->state = RSA_STATE_DECRYPT_RES;
                key->data = NULL;
                return NitroxRsaSSL_Verify(in, inLen, out, &key->dataLen, key);
            }
        }
    #endif

        /* verify the tmp ptr is NULL, otherwise indicates bad state */
        if (key->data != NULL) {
            ret = BAD_STATE_E;
            break;
        }

        /* if not doing this inline then allocate a buffer for it */
        if (outPtr == NULL) {
            key->data = (byte*)XMALLOC(inLen, key->heap, DYNAMIC_TYPE_WOLF_BIGINT);
            key->dataIsAlloc = 1;
            if (key->data == NULL) {
                ret = MEMORY_E;
                break;
            }
            XMEMCPY(key->data, in, inLen);
        }
        else {
            key->data = out;
        }
        ret = wc_RsaFunction(key->data, inLen, key->data, &key->dataLen, rsa_type,
                                                                      key, rng);

        if (ret >= 0 || ret == WC_PENDING_E) {
            key->state = RSA_STATE_DECRYPT_UNPAD;
        }
        if (ret < 0) {
            break;
        }

        FALL_THROUGH;

    case RSA_STATE_DECRYPT_UNPAD:
    {
        byte* pad = NULL;
        ret = wc_RsaUnPad_ex(key->data, key->dataLen, &pad, pad_value, pad_type,
                             hash, mgf, label, labelSz, saltLen,
                             mp_count_bits(&key->n), key->heap);
        if (ret > 0 && ret <= (int)outLen && pad != NULL) {
            /* only copy output if not inline */
            if (outPtr == NULL) {
                XMEMCPY(out, pad, ret);
            }
            else {
                *outPtr = pad;
            }
        }
        else if (ret >= 0) {
            ret = RSA_BUFFER_E;
        }
        if (ret < 0) {
            break;
        }

        key->state = RSA_STATE_DECRYPT_RES;

        FALL_THROUGH;
    }
    case RSA_STATE_DECRYPT_RES:
    #if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA) && \
            defined(HAVE_CAVIUM)
        if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA &&
                                                   pad_type != WC_RSA_PSS_PAD) {
            /* convert result */
            byte* dataLen = (byte*)&key->dataLen;
            ret = (dataLen[0] << 8) | (dataLen[1]);

            if (outPtr)
                *outPtr = in;
        }
    #endif
        break;

    default:
        ret = BAD_STATE_E;
        break;
    }

    /* if async pending then return and skip done cleanup below */
    if (ret == WC_PENDING_E) {
        return ret;
    }

    key->state = RSA_STATE_NONE;
    wc_RsaCleanup(key);

    return ret;
}


/* Public RSA Functions */
int wc_RsaPublicEncrypt(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                     RsaKey* key, WC_RNG* rng)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PUBLIC_ENCRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}


#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_NO_PADDING)
int wc_RsaPublicEncrypt_ex(const byte* in, word32 inLen, byte* out,
                    word32 outLen, RsaKey* key, WC_RNG* rng, int type,
                    enum wc_HashType hash, int mgf, byte* label,
                    word32 labelSz)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key, RSA_PUBLIC_ENCRYPT,
        RSA_BLOCK_TYPE_2, type, hash, mgf, label, labelSz, 0, rng);
}
#endif /* WC_NO_RSA_OAEP */


int wc_RsaPrivateDecryptInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}


#ifndef WC_NO_RSA_OAEP
int wc_RsaPrivateDecryptInline_ex(byte* in, word32 inLen, byte** out,
                                  RsaKey* key, int type, enum wc_HashType hash,
                                  int mgf, byte* label, word32 labelSz)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, type, hash,
        mgf, label, labelSz, 0, rng);
}
#endif /* WC_NO_RSA_OAEP */


int wc_RsaPrivateDecrypt(const byte* in, word32 inLen, byte* out,
                                                 word32 outLen, RsaKey* key)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx((byte*)in, inLen, out, outLen, NULL, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}

#if !defined(WC_NO_RSA_OAEP) || defined(WC_RSA_NO_PADDING)
int wc_RsaPrivateDecrypt_ex(const byte* in, word32 inLen, byte* out,
                            word32 outLen, RsaKey* key, int type,
                            enum wc_HashType hash, int mgf, byte* label,
                            word32 labelSz)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx((byte*)in, inLen, out, outLen, NULL, key,
        RSA_PRIVATE_DECRYPT, RSA_BLOCK_TYPE_2, type, hash, mgf, label,
        labelSz, 0, rng);
}
#endif /* WC_NO_RSA_OAEP || WC_RSA_NO_PADDING */


int wc_RsaSSL_VerifyInline(byte* in, word32 inLen, byte** out, RsaKey* key)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}

int wc_RsaSSL_Verify(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                                 RsaKey* key)
{
    WC_RNG* rng;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx((byte*)in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}

#ifdef WC_RSA_PSS
/* Verify the message signed with RSA-PSS.
 * The input buffer is reused for the ouput buffer.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyInline(byte* in, word32 inLen, byte** out,
                           enum wc_HashType hash, int mgf, RsaKey* key)
{
    return wc_RsaPSS_VerifyInline_ex(in, inLen, out, hash, mgf, -1, key);
}

/* Verify the message signed with RSA-PSS.
 * The input buffer is reused for the ouput buffer.
 *
 * in       Buffer holding encrypted data.
 * inLen    Length of data in buffer.
 * out      Pointer to address containing the PSS data.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * key      Public RSA key.
 * saltLen  Length of salt used. -1 indicates salt length is the same as the
 *          hash length.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_VerifyInline_ex(byte* in, word32 inLen, byte** out,
                              enum wc_HashType hash, int mgf, int saltLen,
                              RsaKey* key)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx(in, inLen, in, inLen, out, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
}

/* Verify the message signed with RSA-PSS.
 * Salt length is equal to hash length.
 *
 * in     Buffer holding encrypted data.
 * inLen  Length of data in buffer.
 * out    Pointer to address containing the PSS data.
 * hash   Hash algorithm.
 * mgf    Mask generation function.
 * key    Public RSA key.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_Verify(byte* in, word32 inLen, byte* out, word32 outLen,
                     enum wc_HashType hash, int mgf, RsaKey* key)
{
    return wc_RsaPSS_Verify_ex(in, inLen, out, outLen, hash, mgf, -1, key);
}

/* Verify the message signed with RSA-PSS.
 *
 * in       Buffer holding encrypted data.
 * inLen    Length of data in buffer.
 * out      Pointer to address containing the PSS data.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * key      Public RSA key.
 * saltLen  Length of salt used. -1 indicates salt length is the same as the
 *          hash length.
 * returns the length of the PSS data on success and negative indicates failure.
 */
int wc_RsaPSS_Verify_ex(byte* in, word32 inLen, byte* out, word32 outLen,
                        enum wc_HashType hash, int mgf, int saltLen,
                        RsaKey* key)
{
    WC_RNG* rng = NULL;
#ifdef WC_RSA_BLINDING
    rng = key->rng;
#endif
    return RsaPrivateDecryptEx(in, inLen, out, outLen, NULL, key,
        RSA_PUBLIC_DECRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
}


/* Checks the PSS data to ensure that the signature matches.
 * Salt length is equal to hash length.
 *
 * in        Hash of the data that is being verified.
 * inSz      Length of hash.
 * sig       Buffer holding PSS data.
 * sigSz     Size of PSS data.
 * hashType  Hash algorithm.
 * returns BAD_PADDING_E when the PSS data is invalid, BAD_FUNC_ARG when
 * NULL is passed in to in or sig or inSz is not the same as the hash
 * algorithm length and 0 on success.
 */
int wc_RsaPSS_CheckPadding(const byte* in, word32 inSz, byte* sig,
                           word32 sigSz, enum wc_HashType hashType)
{
    return wc_RsaPSS_CheckPadding_ex(in, inSz, sig, sigSz, hashType, inSz);
}

/* Checks the PSS data to ensure that the signature matches.
 *
 * in        Hash of the data that is being verified.
 * inSz      Length of hash.
 * sig       Buffer holding PSS data.
 * sigSz     Size of PSS data.
 * hashType  Hash algorithm.
 * saltLen   Length of salt used. -1 indicates salt length is the same as the
 *           hash length.
 * returns BAD_PADDING_E when the PSS data is invalid, BAD_FUNC_ARG when
 * NULL is passed in to in or sig or inSz is not the same as the hash
 * algorithm length and 0 on success.
 */
int wc_RsaPSS_CheckPadding_ex(const byte* in, word32 inSz, byte* sig,
                              word32 sigSz, enum wc_HashType hashType,
                              int saltLen)
{
    int ret = 0;

    if (in == NULL || sig == NULL ||
                      inSz != (word32)wc_HashGetDigestSize(hashType))
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (saltLen == -1)
            saltLen = inSz;
        else if (saltLen < -1 || (word32)saltLen > inSz)
            ret = PSS_SALTLEN_E;
    }
    /* Sig = 8 * 0x00 | Space for Message Hash | Salt | Exp Hash */
    if (ret == 0) {
        if (sigSz != RSA_PSS_PAD_SZ + inSz + (word32)saltLen + inSz)
            ret = BAD_PADDING_E;
    }
    /* Exp Hash = HASH(8 * 0x00 | Message Hash | Salt) */
    if (ret == 0) {
        XMEMCPY(sig + RSA_PSS_PAD_SZ, in, inSz);
        ret = wc_Hash(hashType, sig, RSA_PSS_PAD_SZ + inSz + saltLen, sig,
                      inSz);
    }
    if (ret == 0) {
        if (XMEMCMP(sig, sig + RSA_PSS_PAD_SZ + inSz + saltLen, inSz) != 0) {
            WOLFSSL_MSG("RsaPSS_CheckPadding: Padding Error");
            ret = BAD_PADDING_E;
        }
    }

    return ret;
}
#endif

int wc_RsaSSL_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                                                   RsaKey* key, WC_RNG* rng)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PKCSV15_PAD,
        WC_HASH_TYPE_NONE, WC_MGF1NONE, NULL, 0, 0, rng);
}

#ifdef WC_RSA_PSS
/* Sign the hash of a message using RSA-PSS.
 * Salt length is equal to hash length.
 *
 * in      Buffer holding hash of message.
 * inLen   Length of data in buffer (hash length).
 * out     Buffer to write encrypted signature into.
 * outLen  Size of buffer to write to.
 * hash    Hash algorithm.
 * mgf     Mask generation function.
 * key     Public RSA key.
 * rng     Random number generator.
 * returns the length of the encrypted signature on success, a negative value
 * indicates failure.
 */
int wc_RsaPSS_Sign(const byte* in, word32 inLen, byte* out, word32 outLen,
                       enum wc_HashType hash, int mgf, RsaKey* key, WC_RNG* rng)
{
    return wc_RsaPSS_Sign_ex(in, inLen, out, outLen, hash, mgf, -1, key, rng);
}

/* Sign the hash of a message using RSA-PSS.
 *
 * in       Buffer holding hash of message.
 * inLen    Length of data in buffer (hash length).
 * out      Buffer to write encrypted signature into.
 * outLen   Size of buffer to write to.
 * hash     Hash algorithm.
 * mgf      Mask generation function.
 * saltLen  Length of salt used. -1 indicates salt length is the same as the
 *          hash length.
 * key      Public RSA key.
 * rng      Random number generator.
 * returns the length of the encrypted signature on success, a negative value
 * indicates failure.
 */
int wc_RsaPSS_Sign_ex(const byte* in, word32 inLen, byte* out, word32 outLen,
                      enum wc_HashType hash, int mgf, int saltLen, RsaKey* key,
                      WC_RNG* rng)
{
    return RsaPublicEncryptEx(in, inLen, out, outLen, key,
        RSA_PRIVATE_ENCRYPT, RSA_BLOCK_TYPE_1, WC_RSA_PSS_PAD,
        hash, mgf, NULL, 0, saltLen, rng);
}
#endif

int wc_RsaEncryptSize(RsaKey* key)
{
    int ret;

    if (key == NULL) {
        return BAD_FUNC_ARG;
    }

    ret =  mp_unsigned_bin_size(&key->n);

#ifdef WOLF_CRYPTO_DEV
    if (ret == 0 && key->devId != INVALID_DEVID) {
        ret = 2048/8; /* hardware handles, use 2048-bit as default */
    }
#endif

    return ret;
}


/* flatten RsaKey structure into individual elements (e, n) */
int wc_RsaFlattenPublicKey(RsaKey* key, byte* e, word32* eSz, byte* n,
                                                                   word32* nSz)
{
    int sz, ret;

    if (key == NULL || e == NULL || eSz == NULL || n == NULL || nSz == NULL) {
        return BAD_FUNC_ARG;
    }

    sz = mp_unsigned_bin_size(&key->e);
    if ((word32)sz > *eSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->e, e);
    if (ret != MP_OKAY)
        return ret;
    *eSz = (word32)sz;

    sz = wc_RsaEncryptSize(key);
    if ((word32)sz > *nSz)
        return RSA_BUFFER_E;
    ret = mp_to_unsigned_bin(&key->n, n);
    if (ret != MP_OKAY)
        return ret;
    *nSz = (word32)sz;

    return 0;
}


static int RsaGetValue(mp_int* in, byte* out, word32* outSz)
{
    word32 sz;
    int ret = 0;

    /* Parameters ensured by calling function. */

    sz = (word32)mp_unsigned_bin_size(in);
    if (sz > *outSz)
        ret = RSA_BUFFER_E;

    if (ret == 0)
        ret = mp_to_unsigned_bin(in, out);

    if (ret == MP_OKAY)
        *outSz = sz;

    return ret;
}


int wc_RsaExportKey(RsaKey* key,
                    byte* e, word32* eSz, byte* n, word32* nSz,
                    byte* d, word32* dSz, byte* p, word32* pSz,
                    byte* q, word32* qSz)
{
    int ret = BAD_FUNC_ARG;

    if (key && e && eSz && n && nSz && d && dSz && p && pSz && q && qSz)
        ret = 0;

    if (ret == 0)
        ret = RsaGetValue(&key->e, e, eSz);
    if (ret == 0)
        ret = RsaGetValue(&key->n, n, nSz);
    if (ret == 0)
        ret = RsaGetValue(&key->d, d, dSz);
    if (ret == 0)
        ret = RsaGetValue(&key->p, p, pSz);
    if (ret == 0)
        ret = RsaGetValue(&key->q, q, qSz);

    return ret;
}


#ifdef WOLFSSL_KEY_GEN

/* Check that |p-q| > 2^((size/2)-100) */
static int wc_CompareDiffPQ(mp_int* p, mp_int* q, int size)
{
    mp_int c, d;
    int ret;

    if (p == NULL || q == NULL)
        return BAD_FUNC_ARG;

    ret = mp_init_multi(&c, &d, NULL, NULL, NULL, NULL);

    /* c = 2^((size/2)-100) */
    if (ret == 0)
        ret = mp_2expt(&c, (size/2)-100);

    /* d = |p-q| */
    if (ret == 0)
        ret = mp_sub(p, q, &d);

    if (ret == 0)
        ret = mp_abs(&d, &d);

    /* compare */
    if (ret == 0)
        ret = mp_cmp(&d, &c);

    if (ret == MP_GT)
        ret = MP_OKAY;

    mp_clear(&d);
    mp_clear(&c);

    return ret;
}


/* The lower_bound value is floor(2^(0.5) * 2^((nlen/2)-1)) where nlen is 4096.
 * This number was calculated using a small test tool written with a common
 * large number math library. Other values of nlen may be checked with a subset
 * of lower_bound. */
static const byte lower_bound[] = {
    0xB5, 0x04, 0xF3, 0x33, 0xF9, 0xDE, 0x64, 0x84,
    0x59, 0x7D, 0x89, 0xB3, 0x75, 0x4A, 0xBE, 0x9F,
    0x1D, 0x6F, 0x60, 0xBA, 0x89, 0x3B, 0xA8, 0x4C,
    0xED, 0x17, 0xAC, 0x85, 0x83, 0x33, 0x99, 0x15,
/* 512 */
    0x4A, 0xFC, 0x83, 0x04, 0x3A, 0xB8, 0xA2, 0xC3,
    0xA8, 0xB1, 0xFE, 0x6F, 0xDC, 0x83, 0xDB, 0x39,
    0x0F, 0x74, 0xA8, 0x5E, 0x43, 0x9C, 0x7B, 0x4A,
    0x78, 0x04, 0x87, 0x36, 0x3D, 0xFA, 0x27, 0x68,
/* 1024 */
    0xD2, 0x20, 0x2E, 0x87, 0x42, 0xAF, 0x1F, 0x4E,
    0x53, 0x05, 0x9C, 0x60, 0x11, 0xBC, 0x33, 0x7B,
    0xCA, 0xB1, 0xBC, 0x91, 0x16, 0x88, 0x45, 0x8A,
    0x46, 0x0A, 0xBC, 0x72, 0x2F, 0x7C, 0x4E, 0x33,
    0xC6, 0xD5, 0xA8, 0xA3, 0x8B, 0xB7, 0xE9, 0xDC,
    0xCB, 0x2A, 0x63, 0x43, 0x31, 0xF3, 0xC8, 0x4D,
    0xF5, 0x2F, 0x12, 0x0F, 0x83, 0x6E, 0x58, 0x2E,
    0xEA, 0xA4, 0xA0, 0x89, 0x90, 0x40, 0xCA, 0x4A,
/* 2048 */
    0x81, 0x39, 0x4A, 0xB6, 0xD8, 0xFD, 0x0E, 0xFD,
    0xF4, 0xD3, 0xA0, 0x2C, 0xEB, 0xC9, 0x3E, 0x0C,
    0x42, 0x64, 0xDA, 0xBC, 0xD5, 0x28, 0xB6, 0x51,
    0xB8, 0xCF, 0x34, 0x1B, 0x6F, 0x82, 0x36, 0xC7,
    0x01, 0x04, 0xDC, 0x01, 0xFE, 0x32, 0x35, 0x2F,
    0x33, 0x2A, 0x5E, 0x9F, 0x7B, 0xDA, 0x1E, 0xBF,
    0xF6, 0xA1, 0xBE, 0x3F, 0xCA, 0x22, 0x13, 0x07,
    0xDE, 0xA0, 0x62, 0x41, 0xF7, 0xAA, 0x81, 0xC2,
/* 3072 */
    0xC1, 0xFC, 0xBD, 0xDE, 0xA2, 0xF7, 0xDC, 0x33,
    0x18, 0x83, 0x8A, 0x2E, 0xAF, 0xF5, 0xF3, 0xB2,
    0xD2, 0x4F, 0x4A, 0x76, 0x3F, 0xAC, 0xB8, 0x82,
    0xFD, 0xFE, 0x17, 0x0F, 0xD3, 0xB1, 0xF7, 0x80,
    0xF9, 0xAC, 0xCE, 0x41, 0x79, 0x7F, 0x28, 0x05,
    0xC2, 0x46, 0x78, 0x5E, 0x92, 0x95, 0x70, 0x23,
    0x5F, 0xCF, 0x8F, 0x7B, 0xCA, 0x3E, 0xA3, 0x3B,
    0x4D, 0x7C, 0x60, 0xA5, 0xE6, 0x33, 0xE3, 0xE1
/* 4096 */
};


/* returns 1 on key size ok and 0 if not ok */
static INLINE int RsaSizeCheck(int size)
{
    if (size < RSA_MIN_SIZE || size > RSA_MAX_SIZE) {
        return 0;
    }

#ifdef HAVE_FIPS
    /* Key size requirements for CAVP */
    switch (size) {
        case 1024:
        case 2048:
        case 3072:
        case 4096:
            return 1;
    }

    return 0;
#else
    return 1; /* allow unusual key sizes in non FIPS mode */
#endif /* HAVE_FIPS */
}


static int wc_CheckProbablePrime_ex(mp_int* p, mp_int* q, mp_int* e, int nlen,
                                    int* isPrime)
{
    int ret;
    mp_int tmp1, tmp2;
    mp_int* prime;

    if (p == NULL || e == NULL || isPrime == NULL)
        return BAD_FUNC_ARG;

    if (!RsaSizeCheck(nlen))
        return BAD_FUNC_ARG;

    *isPrime = MP_NO;

    if (q != NULL) {
        /* 5.4 - check that |p-q| <= (2^(1/2))(2^((nlen/2)-1)) */
        ret = wc_CompareDiffPQ(p, q, nlen);
        if (ret != MP_OKAY) goto notOkay;
        prime = q;
    }
    else
        prime = p;

    ret = mp_init_multi(&tmp1, &tmp2, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY) goto notOkay;

    /* 4.4,5.5 - Check that prime >= (2^(1/2))(2^((nlen/2)-1))
     *           This is a comparison against lowerBound */
    ret = mp_read_unsigned_bin(&tmp1, lower_bound, nlen/16);
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp(prime, &tmp1);
    if (ret == MP_LT) goto exit;

    /* 4.5,5.6 - Check that GCD(p-1, e) == 1 */
    ret = mp_sub_d(prime, 1, &tmp1);  /* tmp1 = prime-1 */
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_gcd(&tmp1, e, &tmp2);  /* tmp2 = gcd(prime-1, e) */
    if (ret != MP_OKAY) goto notOkay;
    ret = mp_cmp_d(&tmp2, 1);
    if (ret != MP_EQ) goto exit; /* e divides p-1 */

    /* 4.5.1,5.6.1 - Check primality of p with 8 iterations */
    ret = mp_prime_is_prime(prime, 8, isPrime);
        /* Performs some divides by a table of primes, and then does M-R,
         * it sets isPrime as a side-effect. */
    if (ret != MP_OKAY) goto notOkay;

exit:
    ret = MP_OKAY;
notOkay:
    mp_clear(&tmp1);
    mp_clear(&tmp2);
    return ret;
}



int wc_CheckProbablePrime(const byte* pRaw, word32 pRawSz,
                          const byte* qRaw, word32 qRawSz,
                          const byte* eRaw, word32 eRawSz,
                          int nlen, int* isPrime)
{
    mp_int p, q, e;
    mp_int* Q = NULL;
    int ret;

    if (pRaw == NULL || pRawSz == 0 ||
        eRaw == NULL || eRawSz == 0 ||
        isPrime == NULL) {

        return BAD_FUNC_ARG;
    }

    if ((qRaw != NULL && qRawSz == 0) || (qRaw == NULL && qRawSz != 0))
        return BAD_FUNC_ARG;

    ret = mp_init_multi(&p, &q, &e, NULL, NULL, NULL);

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(&p, pRaw, pRawSz);

    if (ret == MP_OKAY) {
        if (qRaw != NULL) {
            ret = mp_read_unsigned_bin(&q, qRaw, qRawSz);
            if (ret == MP_OKAY)
                Q = &q;
        }
    }

    if (ret == MP_OKAY)
        ret = mp_read_unsigned_bin(&e, eRaw, eRawSz);

    if (ret == MP_OKAY)
        ret = wc_CheckProbablePrime_ex(&p, Q, &e, nlen, isPrime);

    ret = (ret == MP_OKAY) ? 0 : PRIME_GEN_E;

    mp_clear(&p);
    mp_clear(&q);
    mp_clear(&e);

    return ret;
}


/* Make an RSA key for size bits, with e specified, 65537 is a good e */
int wc_MakeRsaKey(RsaKey* key, int size, long e, WC_RNG* rng)
{
    mp_int p, q, tmp1, tmp2, tmp3;
    int err, i, failCount, primeSz, isPrime;
    byte* buf = NULL;

    if (key == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    if (!RsaSizeCheck(size))
        return BAD_FUNC_ARG;

    if (e < 3 || (e & 1) == 0)
        return BAD_FUNC_ARG;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_RSA)
    if (key->asyncDev.marker == WOLFSSL_ASYNC_MARKER_RSA) {
    #ifdef HAVE_CAVIUM
        /* TODO: Not implemented */
    #elif defined(HAVE_INTEL_QA)
        /* TODO: Not implemented */
    #else
        if (wc_AsyncTestInit(&key->asyncDev, ASYNC_TEST_RSA_MAKE)) {
            WC_ASYNC_TEST* testDev = &key->asyncDev.test;
            testDev->rsaMake.rng = rng;
            testDev->rsaMake.key = key;
            testDev->rsaMake.size = size;
            testDev->rsaMake.e = e;
            return WC_PENDING_E;
        }
    #endif
    }
#endif

    err = mp_init_multi(&p, &q, &tmp1, &tmp2, &tmp3, NULL);

    if (err == MP_OKAY)
        err = mp_set_int(&tmp3, e);

    /* The failCount value comes from NIST FIPS 186-4, section B.3.3,
     * process steps 4.7 and 5.8. */
    failCount = 5 * (size / 2);
    primeSz = size / 16; /* size is the size of n in bits.
                            primeSz is in bytes. */

    /* allocate buffer to work with */
    if (err == MP_OKAY) {
        buf = (byte*)XMALLOC(primeSz, key->heap, DYNAMIC_TYPE_RSA);
        if (buf == NULL)
            err = MEMORY_E;
    }

    /* make p */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);

            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(&p, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = wc_CheckProbablePrime_ex(&p, NULL, &tmp3, size, &isPrime);

#ifdef WOLFSSL_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    /* make q */
    if (err == MP_OKAY) {
        isPrime = 0;
        i = 0;
        do {
#ifdef SHOW_GEN
            printf(".");
            fflush(stdout);
#endif
            /* generate value */
            err = wc_RNG_GenerateBlock(rng, buf, primeSz);

            if (err == 0) {
                /* prime lower bound has the MSB set, set it in candidate */
                buf[0] |= 0x80;
                /* make candidate odd */
                buf[primeSz-1] |= 0x01;
                /* load value */
                err = mp_read_unsigned_bin(&q, buf, primeSz);
            }

            if (err == MP_OKAY)
                err = wc_CheckProbablePrime_ex(&p, &q, &tmp3, size, &isPrime);

#ifdef WOLFSSL_FIPS
            i++;
#else
            /* Keep the old retry behavior in non-FIPS build. */
            (void)i;
#endif
        } while (err == MP_OKAY && !isPrime && i < failCount);
    }

    if (err == MP_OKAY && !isPrime)
        err = PRIME_GEN_E;

    if (buf) {
        ForceZero(buf, primeSz);
        XFREE(buf, key->heap, DYNAMIC_TYPE_RSA);
    }

    if (err == MP_OKAY)
        err = mp_init_multi(&key->n, &key->e, &key->d, &key->p, &key->q, NULL);

    if (err == MP_OKAY)
        err = mp_init_multi(&key->dP, &key->dQ, &key->u, NULL, NULL, NULL);

    if (err == MP_OKAY)
        err = mp_sub_d(&p, 1, &tmp1);  /* tmp1 = p-1 */

    if (err == MP_OKAY)
        err = mp_sub_d(&q, 1, &tmp2);  /* tmp2 = q-1 */

    if (err == MP_OKAY)
        err = mp_lcm(&tmp1, &tmp2, &tmp3);  /* tmp3 = lcm(p-1, q-1),last loop */

    /* make key */
    if (err == MP_OKAY)
        err = mp_set_int(&key->e, (mp_digit)e);  /* key->e = e */

    if (err == MP_OKAY)                /* key->d = 1/e mod lcm(p-1, q-1) */
        err = mp_invmod(&key->e, &tmp3, &key->d);

    if (err == MP_OKAY)
        err = mp_mul(&p, &q, &key->n);  /* key->n = pq */

    if (err == MP_OKAY)
        err = mp_mod(&key->d, &tmp1, &key->dP); /* key->dP = d mod(p-1) */

    if (err == MP_OKAY)
        err = mp_mod(&key->d, &tmp2, &key->dQ); /* key->dQ = d mod(q-1) */

    if (err == MP_OKAY)
        err = mp_invmod(&q, &p, &key->u); /* key->u = 1/q mod p */

    if (err == MP_OKAY)
        err = mp_copy(&p, &key->p);

    if (err == MP_OKAY)
        err = mp_copy(&q, &key->q);

    if (err == MP_OKAY)
        key->type = RSA_PRIVATE;

    mp_clear(&tmp1);
    mp_clear(&tmp2);
    mp_clear(&tmp3);
    mp_clear(&p);
    mp_clear(&q);

    if (err != MP_OKAY) {
        wc_FreeRsaKey(key);
        return err;
    }

#ifdef WOLFSSL_XILINX_CRYPT
    if (wc_InitRsaHw(key) != 0) {
        return BAD_STATE_E;
    }
#endif

    return 0;
}
#endif /* WOLFSSL_KEY_GEN */


#ifdef WC_RSA_BLINDING

int wc_RsaSetRNG(RsaKey* key, WC_RNG* rng)
{
    if (key == NULL)
        return BAD_FUNC_ARG;

    key->rng = rng;

    return 0;
}

#endif /* WC_RSA_BLINDING */


#undef ERROR_OUT

#endif /* HAVE_FIPS */
#endif /* NO_RSA */
