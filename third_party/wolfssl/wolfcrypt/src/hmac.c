/* hmac.h
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
#include <wolfssl/wolfcrypt/error-crypt.h>

#ifndef NO_HMAC

#include <wolfssl/wolfcrypt/hmac.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


/* fips wrapper calls, user can call direct */
#ifdef HAVE_FIPS
    /* does init */
    int wc_HmacSetKey(Hmac* hmac, int type, const byte* key, word32 keySz)
    {
        if (hmac == NULL || (key == NULL && keySz != 0) ||
           !(type == MD5 || type == SHA    || type == SHA256 || type == SHA384
                         || type == SHA512 || type == BLAKE2B_ID)) {
            return BAD_FUNC_ARG;
        }

        return HmacSetKey_fips(hmac, type, key, keySz);
    }
    int wc_HmacUpdate(Hmac* hmac, const byte* in, word32 sz)
    {
        if (hmac == NULL || (in == NULL && sz > 0)) {
            return BAD_FUNC_ARG;
        }

        return HmacUpdate_fips(hmac, in, sz);
    }
    int wc_HmacFinal(Hmac* hmac, byte* out)
    {
        if (hmac == NULL) {
            return BAD_FUNC_ARG;
        }

        return HmacFinal_fips(hmac, out);
    }
    int wolfSSL_GetHmacMaxSize(void)
    {
        return CyaSSL_GetHmacMaxSize();
    }

    int wc_HmacInit(Hmac* hmac, void* heap, int devId)
    {
        (void)hmac;
        (void)heap;
        (void)devId;
        /* FIPS doesn't support:
            return HmacInit(hmac, heap, devId); */
        return 0;
    }
    void wc_HmacFree(Hmac* hmac)
    {
        (void)hmac;
        /* FIPS doesn't support:
            HmacFree(hmac); */
    }

    #ifdef HAVE_HKDF
        int wc_HKDF(int type, const byte* inKey, word32 inKeySz,
                    const byte* salt, word32 saltSz,
                    const byte* info, word32 infoSz,
                    byte* out, word32 outSz)
        {
            return HKDF(type, inKey, inKeySz, salt, saltSz,
                info, infoSz, out, outSz);
        }
    #endif /* HAVE_HKDF */

#else /* else build without fips */


#include <wolfssl/wolfcrypt/error-crypt.h>


int wc_HmacSizeByType(int type)
{
    int ret;

    if (!(type == MD5 || type == SHA    || type == SHA256 || type == SHA384
                      || type == SHA512 || type == BLAKE2B_ID
                      || type == SHA224)) {
        return BAD_FUNC_ARG;
    }

    switch (type) {
    #ifndef NO_MD5
        case MD5:
            ret = MD5_DIGEST_SIZE;
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            ret = SHA_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
            ret = SHA224_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
            ret = SHA256_DIGEST_SIZE;
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            ret = SHA384_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
            ret = SHA512_DIGEST_SIZE;
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            ret = BLAKE2B_OUTBYTES;
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    return ret;
}

static int _InitHmac(Hmac* hmac, int type, void* heap)
{
    int ret = 0;

    switch (type) {
    #ifndef NO_MD5
        case MD5:
            ret = wc_InitMd5(&hmac->hash.md5);
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            ret = wc_InitSha(&hmac->hash.sha);
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
            ret = wc_InitSha224(&hmac->hash.sha224);
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
            ret = wc_InitSha256(&hmac->hash.sha256);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            ret = wc_InitSha384(&hmac->hash.sha384);
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
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

    /* default to NULL heap hint or test value */
#ifdef WOLFSSL_HEAP_TEST
    hmac->heap = (void)WOLFSSL_HEAP_TEST;
#else
    hmac->heap = heap;
#endif /* WOLFSSL_HEAP_TEST */

    return ret;
}


int wc_HmacSetKey(Hmac* hmac, int type, const byte* key, word32 length)
{
    byte*  ip;
    byte*  op;
    word32 i, hmac_block_size = 0;
    int    ret = 0;
    void*  heap = NULL;

    if (hmac == NULL || (key == NULL && length != 0) ||
        !(type == MD5 || type == SHA    || type == SHA256 || type == SHA384
                      || type == SHA512 || type == BLAKE2B_ID
                      || type == SHA224)) {
        return BAD_FUNC_ARG;
    }

    hmac->innerHashKeyed = 0;
    hmac->macType = (byte)type;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    if (hmac->asyncDev.marker == WOLFSSL_ASYNC_MARKER_HMAC) {
    #if defined(HAVE_CAVIUM)
        if (length > HMAC_BLOCK_SIZE) {
            return WC_KEY_SIZE_E;
        }

        if (key != NULL) {
            XMEMCPY(hmac->ipad, key, length);
        }
        hmac->keyLen = (word16)length;

        return 0; /* nothing to do here */
    #endif /* HAVE_CAVIUM */
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    ret = _InitHmac(hmac, type, heap);
    if (ret != 0)
        return ret;

#ifdef HAVE_FIPS
    if (length < HMAC_FIPS_MIN_KEY)
        return HMAC_MIN_KEYLEN_E;
#endif

    ip = (byte*)hmac->ipad;
    op = (byte*)hmac->opad;

    switch (hmac->macType) {
    #ifndef NO_MD5
        case MD5:
            hmac_block_size = MD5_BLOCK_SIZE;
            if (length <= MD5_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Md5Update(&hmac->hash.md5, key, length);
                if (ret != 0)
                    break;
                ret = wc_Md5Final(&hmac->hash.md5, ip);
                if (ret != 0)
                    break;
                length = MD5_DIGEST_SIZE;
            }
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            hmac_block_size = SHA_BLOCK_SIZE;
            if (length <= SHA_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_ShaUpdate(&hmac->hash.sha, key, length);
                if (ret != 0)
                    break;
                ret = wc_ShaFinal(&hmac->hash.sha, ip);
                if (ret != 0)
                    break;

                length = SHA_DIGEST_SIZE;
            }
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
        {
            hmac_block_size = SHA224_BLOCK_SIZE;
            if (length <= SHA224_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Sha224Update(&hmac->hash.sha224, key, length);
                if (ret != 0)
                    break;
                ret = wc_Sha224Final(&hmac->hash.sha224, ip);
                if (ret != 0)
                    break;

                length = SHA224_DIGEST_SIZE;
            }
        }
        break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
    		hmac_block_size = SHA256_BLOCK_SIZE;
            if (length <= SHA256_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Sha256Update(&hmac->hash.sha256, key, length);
                if (ret != 0)
                    break;
                ret = wc_Sha256Final(&hmac->hash.sha256, ip);
                if (ret != 0)
                    break;

                length = SHA256_DIGEST_SIZE;
            }
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            hmac_block_size = SHA384_BLOCK_SIZE;
            if (length <= SHA384_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Sha384Update(&hmac->hash.sha384, key, length);
                if (ret != 0)
                    break;
                ret = wc_Sha384Final(&hmac->hash.sha384, ip);
                if (ret != 0)
                    break;

                length = SHA384_DIGEST_SIZE;
            }
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
            hmac_block_size = SHA512_BLOCK_SIZE;
            if (length <= SHA512_BLOCK_SIZE) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Sha512Update(&hmac->hash.sha512, key, length);
                if (ret != 0)
                    break;
                ret = wc_Sha512Final(&hmac->hash.sha512, ip);
                if (ret != 0)
                    break;

                length = SHA512_DIGEST_SIZE;
            }
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            hmac_block_size = BLAKE2B_BLOCKBYTES;
            if (length <= BLAKE2B_BLOCKBYTES) {
                if (key != NULL) {
                    XMEMCPY(ip, key, length);
                }
            }
            else {
                ret = wc_Blake2bUpdate(&hmac->hash.blake2b, key, length);
                if (ret != 0)
                    break;
                ret = wc_Blake2bFinal(&hmac->hash.blake2b, ip, BLAKE2B_256);
                if (ret != 0)
                    break;

                length = BLAKE2B_256;
            }
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    if (hmac->asyncDev.marker == WOLFSSL_ASYNC_MARKER_HMAC) {
    #if defined(HAVE_INTEL_QA)
        if (length > hmac_block_size)
            length = hmac_block_size;
        /* update key length */
        hmac->keyLen = (word16)length;

        return ret;
        /* no need to pad below */
    #endif
    }
#endif

    if (ret == 0) {
        if (length < hmac_block_size)
            XMEMSET(ip + length, 0, hmac_block_size - length);

        for(i = 0; i < hmac_block_size; i++) {
            op[i] = ip[i] ^ OPAD;
            ip[i] ^= IPAD;
        }
    }

    return ret;
}


static int HmacKeyInnerHash(Hmac* hmac)
{
    int ret = 0;

    switch (hmac->macType) {
    #ifndef NO_MD5
        case MD5:
            ret = wc_Md5Update(&hmac->hash.md5, (byte*)hmac->ipad,
                                                                MD5_BLOCK_SIZE);
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            ret = wc_ShaUpdate(&hmac->hash.sha, (byte*)hmac->ipad,
                                                                SHA_BLOCK_SIZE);
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
            ret = wc_Sha224Update(&hmac->hash.sha224, (byte*)hmac->ipad,
                                                             SHA224_BLOCK_SIZE);
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
            ret = wc_Sha256Update(&hmac->hash.sha256, (byte*)hmac->ipad,
                                                             SHA256_BLOCK_SIZE);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            ret = wc_Sha384Update(&hmac->hash.sha384, (byte*)hmac->ipad,
                                                             SHA384_BLOCK_SIZE);
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
            ret = wc_Sha512Update(&hmac->hash.sha512, (byte*)hmac->ipad,
                                                             SHA512_BLOCK_SIZE);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            ret = wc_Blake2bUpdate(&hmac->hash.blake2b, (byte*)hmac->ipad,
                                                            BLAKE2B_BLOCKBYTES);
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            break;
    }

    if (ret == 0)
        hmac->innerHashKeyed = 1;

    return ret;
}


int wc_HmacUpdate(Hmac* hmac, const byte* msg, word32 length)
{
    int ret = 0;

    if (hmac == NULL || (msg == NULL && length > 0)) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    if (hmac->asyncDev.marker == WOLFSSL_ASYNC_MARKER_HMAC) {
    #if defined(HAVE_CAVIUM)
        return NitroxHmacUpdate(hmac, msg, length);
    #elif defined(HAVE_INTEL_QA)
        return IntelQaHmac(&hmac->asyncDev, hmac->macType,
            (byte*)hmac->ipad, hmac->keyLen, NULL, msg, length);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    if (!hmac->innerHashKeyed) {
        ret = HmacKeyInnerHash(hmac);
        if (ret != 0)
            return ret;
    }

    switch (hmac->macType) {
    #ifndef NO_MD5
        case MD5:
            ret = wc_Md5Update(&hmac->hash.md5, msg, length);
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            ret = wc_ShaUpdate(&hmac->hash.sha, msg, length);
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
            ret = wc_Sha224Update(&hmac->hash.sha224, msg, length);
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
            ret = wc_Sha256Update(&hmac->hash.sha256, msg, length);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            ret = wc_Sha384Update(&hmac->hash.sha384, msg, length);
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
            ret = wc_Sha512Update(&hmac->hash.sha512, msg, length);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            ret = wc_Blake2bUpdate(&hmac->hash.blake2b, msg, length);
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            break;
    }

    return ret;
}


int wc_HmacFinal(Hmac* hmac, byte* hash)
{
    int ret;

    if (hmac == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    if (hmac->asyncDev.marker == WOLFSSL_ASYNC_MARKER_HMAC) {
        int hashLen = wc_HmacSizeByType(hmac->macType);
        if (hashLen <= 0)
            return hashLen;

    #if defined(HAVE_CAVIUM)
        return NitroxHmacFinal(hmac, hmac->macType, hash, hashLen);
    #elif defined(HAVE_INTEL_QA)
        return IntelQaHmac(&hmac->asyncDev, hmac->macType,
            (byte*)hmac->ipad, hmac->keyLen, hash, NULL, hashLen);
    #endif
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    if (!hmac->innerHashKeyed) {
        ret = HmacKeyInnerHash(hmac);
        if (ret != 0)
            return ret;
    }

    switch (hmac->macType) {
    #ifndef NO_MD5
        case MD5:
            ret = wc_Md5Final(&hmac->hash.md5, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_Md5Update(&hmac->hash.md5, (byte*)hmac->opad,
                                                                MD5_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_Md5Update(&hmac->hash.md5, (byte*)hmac->innerHash,
                                                               MD5_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_Md5Final(&hmac->hash.md5, hash);
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case SHA:
            ret = wc_ShaFinal(&hmac->hash.sha, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_ShaUpdate(&hmac->hash.sha, (byte*)hmac->opad,
                                                                SHA_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_ShaUpdate(&hmac->hash.sha, (byte*)hmac->innerHash,
                                                               SHA_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_ShaFinal(&hmac->hash.sha, hash);
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case SHA224:
        {
            ret = wc_Sha224Final(&hmac->hash.sha224, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_Sha224Update(&hmac->hash.sha224, (byte*)hmac->opad,
                                                             SHA224_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha224Update(&hmac->hash.sha224, (byte*)hmac->innerHash,
                                                            SHA224_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha224Final(&hmac->hash.sha224, hash);
            if (ret != 0)
                break;
        }
        break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case SHA256:
            ret = wc_Sha256Final(&hmac->hash.sha256, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_Sha256Update(&hmac->hash.sha256, (byte*)hmac->opad,
                                                             SHA256_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha256Update(&hmac->hash.sha256, (byte*)hmac->innerHash,
                                                            SHA256_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha256Final(&hmac->hash.sha256, hash);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case SHA384:
            ret = wc_Sha384Final(&hmac->hash.sha384, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_Sha384Update(&hmac->hash.sha384, (byte*)hmac->opad,
                                                             SHA384_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha384Update(&hmac->hash.sha384, (byte*)hmac->innerHash,
                                                            SHA384_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha384Final(&hmac->hash.sha384, hash);
            break;
    #endif /* WOLFSSL_SHA384 */
        case SHA512:
            ret = wc_Sha512Final(&hmac->hash.sha512, (byte*)hmac->innerHash);
            if (ret != 0)
                break;
            ret = wc_Sha512Update(&hmac->hash.sha512, (byte*)hmac->opad,
                                                             SHA512_BLOCK_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha512Update(&hmac->hash.sha512, (byte*)hmac->innerHash,
                                                            SHA512_DIGEST_SIZE);
            if (ret != 0)
                break;
            ret = wc_Sha512Final(&hmac->hash.sha512, hash);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            ret = wc_Blake2bFinal(&hmac->hash.blake2b, (byte*)hmac->innerHash,
                                                                   BLAKE2B_256);
            if (ret != 0)
                break;
            ret = wc_Blake2bUpdate(&hmac->hash.blake2b, (byte*)hmac->opad,
                                                            BLAKE2B_BLOCKBYTES);
            if (ret != 0)
                break;
            ret = wc_Blake2bUpdate(&hmac->hash.blake2b, (byte*)hmac->innerHash,
                                                                   BLAKE2B_256);
            if (ret != 0)
                break;
            ret = wc_Blake2bFinal(&hmac->hash.blake2b, hash, BLAKE2B_256);
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    if (ret == 0) {
        hmac->innerHashKeyed = 0;
    }

    return ret;
}


/* Initialize Hmac for use with async device */
int wc_HmacInit(Hmac* hmac, void* heap, int devId)
{
    int ret = 0;

    if (hmac == NULL)
        return BAD_FUNC_ARG;

    XMEMSET(hmac, 0, sizeof(Hmac));
    hmac->heap = heap;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    hmac->keyLen = 0;
    #ifdef HAVE_CAVIUM
        hmac->dataLen = 0;
        hmac->data    = NULL;        /* buffered input data */
    #endif /* HAVE_CAVIUM */

    ret = wolfAsync_DevCtxInit(&hmac->asyncDev, WOLFSSL_ASYNC_MARKER_HMAC,
                                                         hmac->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}

/* Free Hmac from use with async device */
void wc_HmacFree(Hmac* hmac)
{
    if (hmac == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_HMAC)
    wolfAsync_DevCtxFree(&hmac->asyncDev, WOLFSSL_ASYNC_MARKER_HMAC);

#ifdef HAVE_CAVIUM
    XFREE(hmac->data, hmac->heap, DYNAMIC_TYPE_HMAC);
    hmac->data = NULL;
#endif /* HAVE_CAVIUM */
#endif /* WOLFSSL_ASYNC_CRYPT */
}

int wolfSSL_GetHmacMaxSize(void)
{
    return MAX_DIGEST_SIZE;
}

#ifdef HAVE_HKDF
    /* HMAC-KDF-Extract.
     * RFC 5869 - HMAC-based Extract-and-Expand Key Derivation Function (HKDF).
     *
     * type     The hash algorithm type.
     * salt     The optional salt value.
     * saltSz   The size of the salt.
     * inKey    The input keying material.
     * inKeySz  The size of the input keying material.
     * out      The pseudorandom key with the length that of the hash.
     * returns 0 on success, otherwise failure.
     */
    int wc_HKDF_Extract(int type, const byte* salt, word32 saltSz,
                        const byte* inKey, word32 inKeySz, byte* out)
    {
        byte   tmp[MAX_DIGEST_SIZE]; /* localSalt helper */
        Hmac   myHmac;
        int    ret;
        const  byte* localSalt;  /* either points to user input or tmp */
        int    hashSz;

        ret = wc_HmacSizeByType(type);
        if (ret < 0)
            return ret;

        hashSz = ret;
        localSalt = salt;
        if (localSalt == NULL) {
            XMEMSET(tmp, 0, hashSz);
            localSalt = tmp;
            saltSz    = hashSz;
        }

        ret = wc_HmacInit(&myHmac, NULL, INVALID_DEVID);
        if (ret == 0) {
            ret = wc_HmacSetKey(&myHmac, type, localSalt, saltSz);
            if (ret == 0)
                ret = wc_HmacUpdate(&myHmac, inKey, inKeySz);
            if (ret == 0)
                ret = wc_HmacFinal(&myHmac,  out);
            wc_HmacFree(&myHmac);
        }

        return ret;
    }

    /* HMAC-KDF-Expand.
     * RFC 5869 - HMAC-based Extract-and-Expand Key Derivation Function (HKDF).
     *
     * type     The hash algorithm type.
     * inKey    The input key.
     * inKeySz  The size of the input key.
     * info     The application specific information.
     * infoSz   The size of the application specific information.
     * out      The output keying material.
     * returns 0 on success, otherwise failure.
     */
    int wc_HKDF_Expand(int type, const byte* inKey, word32 inKeySz,
                       const byte* info, word32 infoSz, byte* out, word32 outSz)
    {
        byte   tmp[MAX_DIGEST_SIZE];
        Hmac   myHmac;
        int    ret = 0;
        word32 outIdx = 0;
        word32 hashSz = wc_HmacSizeByType(type);
        byte   n = 0x1;

        ret = wc_HmacInit(&myHmac, NULL, INVALID_DEVID);
        if (ret != 0)
            return ret;

        while (outIdx < outSz) {
            int    tmpSz = (n == 1) ? 0 : hashSz;
            word32 left = outSz - outIdx;

            ret = wc_HmacSetKey(&myHmac, type, inKey, inKeySz);
            if (ret != 0)
                break;
            ret = wc_HmacUpdate(&myHmac, tmp, tmpSz);
            if (ret != 0)
                break;
            ret = wc_HmacUpdate(&myHmac, info, infoSz);
            if (ret != 0)
                break;
            ret = wc_HmacUpdate(&myHmac, &n, 1);
            if (ret != 0)
                break;
            ret = wc_HmacFinal(&myHmac, tmp);
            if (ret != 0)
                break;

            left = min(left, hashSz);
            XMEMCPY(out+outIdx, tmp, left);

            outIdx += hashSz;
            n++;
        }

        wc_HmacFree(&myHmac);

        return ret;
    }

    /* HMAC-KDF.
     * RFC 5869 - HMAC-based Extract-and-Expand Key Derivation Function (HKDF).
     *
     * type     The hash algorithm type.
     * inKey    The input keying material.
     * inKeySz  The size of the input keying material.
     * salt     The optional salt value.
     * saltSz   The size of the salt.
     * info     The application specific information.
     * infoSz   The size of the application specific information.
     * out      The output keying material.
     * returns 0 on success, otherwise failure.
     */
    int wc_HKDF(int type, const byte* inKey, word32 inKeySz,
                       const byte* salt,  word32 saltSz,
                       const byte* info,  word32 infoSz,
                       byte* out,         word32 outSz)
    {
        byte   prk[MAX_DIGEST_SIZE];
        int    hashSz = wc_HmacSizeByType(type);
        int    ret;

        if (hashSz < 0)
            return BAD_FUNC_ARG;

        ret = wc_HKDF_Extract(type, salt, saltSz, inKey, inKeySz, prk);
        if (ret != 0)
            return ret;

        return wc_HKDF_Expand(type, prk, hashSz, info, infoSz, out, outSz);
    }

#endif /* HAVE_HKDF */

#endif /* HAVE_FIPS */
#endif /* NO_HMAC */
