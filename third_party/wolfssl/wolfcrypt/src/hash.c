/* hash.c
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
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#ifndef NO_ASN
#include <wolfssl/wolfcrypt/asn.h>
#endif

#include <wolfssl/wolfcrypt/hash.h>


#if !defined(NO_ASN) || !defined(NO_DH) || defined(HAVE_ECC)

#ifdef NO_ASN
enum Hash_Sum  {
    MD2h    = 646,
    MD5h    = 649,
    SHAh    =  88,
    SHA224h = 417,
    SHA256h = 414,
    SHA384h = 415,
    SHA512h = 416
};
#endif

int wc_HashGetOID(enum wc_HashType hash_type)
{
    int oid = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD2:
        #ifdef WOLFSSL_MD2
            oid = MD2h;
        #endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD5:
        #ifndef NO_MD5
            oid = MD5h;
        #endif
            break;
        case WC_HASH_TYPE_SHA:
        #ifndef NO_SHA
            oid = SHAh;
        #endif
            break;
        case WC_HASH_TYPE_SHA224:
        #if defined(WOLFSSL_SHA224)
            oid = SHA224h;
        #endif
            break;
        case WC_HASH_TYPE_SHA256:
        #ifndef NO_SHA256
            oid = SHA256h;
        #endif
            break;
        case WC_HASH_TYPE_SHA384:
        #if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            oid = SHA384h;
        #endif
            break;
        case WC_HASH_TYPE_SHA512:
        #ifdef WOLFSSL_SHA512
            oid = SHA512h;
        #endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            oid = BAD_FUNC_ARG;
            break;
    }
    return oid;
}
#endif

/* Get Hash digest size */
int wc_HashGetDigestSize(enum wc_HashType hash_type)
{
    int dig_size = HASH_TYPE_E; /* Default to hash type error */
    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            dig_size = MD5_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            dig_size = SHA_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            dig_size = SHA224_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            dig_size = SHA256_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            dig_size = SHA384_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            dig_size = SHA512_DIGEST_SIZE;
#endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
#if !defined(NO_MD5) && !defined(NO_SHA)
            dig_size = MD5_DIGEST_SIZE + SHA_DIGEST_SIZE;
#endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            dig_size = BAD_FUNC_ARG;
            break;
    }
    return dig_size;
}

/* Generic Hashing Wrapper */
int wc_Hash(enum wc_HashType hash_type, const byte* data,
    word32 data_len, byte* hash, word32 hash_len)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */
    word32 dig_size;

    /* Validate hash buffer size */
    dig_size = wc_HashGetDigestSize(hash_type);
    if (hash_len < dig_size) {
        return BUFFER_E;
    }

    /* Suppress possible unused arg if all hashing is disabled */
    (void)data;
    (void)data_len;
    (void)hash;
    (void)hash_len;

    switch(hash_type)
    {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            ret = wc_Md5Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaHash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#if defined(WOLFSSL_SHA512) && defined(WOLFSSL_SHA384)
            ret = wc_Sha384Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Hash(data, data_len, hash);
#endif
            break;
        case WC_HASH_TYPE_MD5_SHA:
#if !defined(NO_MD5) && !defined(NO_SHA)
            ret = wc_Md5Hash(data, data_len, hash);
            if (ret == 0) {
                ret = wc_ShaHash(data, data_len, &hash[MD5_DIGEST_SIZE]);
            }
#endif
            break;

        /* Not Supported */
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
            break;
    }
    return ret;
}

int wc_HashInit(wc_HashAlg* hash, enum wc_HashType type)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_InitMd5(&hash->md5);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_InitSha(&hash->sha);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_InitSha224(&hash->sha224);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_InitSha256(&hash->sha256);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_InitSha384(&hash->sha384);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_InitSha512(&hash->sha512);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashUpdate(wc_HashAlg* hash, enum wc_HashType type, const byte* data,
                  word32 dataSz)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || data == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_Md5Update(&hash->md5, data, dataSz);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaUpdate(&hash->sha, data, dataSz);
            if (ret != 0)
                return ret;
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Update(&hash->sha224, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Update(&hash->sha256, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Update(&hash->sha384, data, dataSz);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Update(&hash->sha512, data, dataSz);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}

int wc_HashFinal(wc_HashAlg* hash, enum wc_HashType type, byte* out)
{
    int ret = HASH_TYPE_E; /* Default to hash type error */

    if (hash == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (type) {
        case WC_HASH_TYPE_MD5:
#ifndef NO_MD5
            wc_Md5Final(&hash->md5, out);
            ret = 0;
#endif
            break;
        case WC_HASH_TYPE_SHA:
#ifndef NO_SHA
            ret = wc_ShaFinal(&hash->sha, out);
#endif
            break;
        case WC_HASH_TYPE_SHA224:
#ifdef WOLFSSL_SHA224
            ret = wc_Sha224Final(&hash->sha224, out);
#endif
            break;
        case WC_HASH_TYPE_SHA256:
#ifndef NO_SHA256
            ret = wc_Sha256Final(&hash->sha256, out);
#endif
            break;
        case WC_HASH_TYPE_SHA384:
#ifdef WOLFSSL_SHA384
            ret = wc_Sha384Final(&hash->sha384, out);
#endif
            break;
        case WC_HASH_TYPE_SHA512:
#ifdef WOLFSSL_SHA512
            ret = wc_Sha512Final(&hash->sha512, out);
#endif
            break;

        /* not supported */
        case WC_HASH_TYPE_MD5_SHA:
        case WC_HASH_TYPE_MD2:
        case WC_HASH_TYPE_MD4:
        case WC_HASH_TYPE_NONE:
        default:
            ret = BAD_FUNC_ARG;
    };

    return ret;
}


#if !defined(WOLFSSL_TI_HASH)

#if !defined(NO_MD5)
    int wc_Md5Hash(const byte* data, word32 len, byte* hash)
    {
        int ret;
    #ifdef WOLFSSL_SMALL_STACK
        Md5* md5;
    #else
        Md5  md5[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        md5 = (Md5*)XMALLOC(sizeof(Md5), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (md5 == NULL)
            return MEMORY_E;
    #endif

        ret = wc_InitMd5(md5);
        if (ret == 0) {
            ret = wc_Md5Update(md5, data, len);
            if (ret == 0) {
                ret = wc_Md5Final(md5, hash);
            }
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(md5, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_MD5 */

#if !defined(NO_SHA)
    int wc_ShaHash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha* sha;
    #else
        Sha sha[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha = (Sha*)XMALLOC(sizeof(Sha), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha(sha)) != 0) {
            WOLFSSL_MSG("wc_InitSha failed");
        }
        else {
            wc_ShaUpdate(sha, data, len);
            wc_ShaFinal(sha, hash);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA */

#if defined(WOLFSSL_SHA224)
int wc_Sha224Hash(const byte* data, word32 len, byte* hash)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    Sha224* sha224;
#else
    Sha224 sha224[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
    sha224 = (Sha224*)XMALLOC(sizeof(Sha224), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (sha224 == NULL)
        return MEMORY_E;
#endif

    if ((ret = wc_InitSha224(sha224)) != 0) {
        WOLFSSL_MSG("InitSha224 failed");
    }
    else if ((ret = wc_Sha224Update(sha224, data, len)) != 0) {
        WOLFSSL_MSG("Sha224Update failed");
    }
    else if ((ret = wc_Sha224Final(sha224, hash)) != 0) {
        WOLFSSL_MSG("Sha224Final failed");
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha224, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif /* WOLFSSL_SHA224 */

#if !defined(NO_SHA256)
    int wc_Sha256Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha256* sha256;
    #else
        Sha256 sha256[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha256 = (Sha256*)XMALLOC(sizeof(Sha256), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha256 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha256(sha256)) != 0) {
            WOLFSSL_MSG("InitSha256 failed");
        }
        else if ((ret = wc_Sha256Update(sha256, data, len)) != 0) {
            WOLFSSL_MSG("Sha256Update failed");
        }
        else if ((ret = wc_Sha256Final(sha256, hash)) != 0) {
            WOLFSSL_MSG("Sha256Final failed");
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }
#endif /* !NO_SHA256 */

#endif /* !defined(WOLFSSL_TI_HASH) */


#if defined(WOLFSSL_SHA512)
    int wc_Sha512Hash(const byte* data, word32 len, byte* hash)
    {
        int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        Sha512* sha512;
    #else
        Sha512 sha512[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        sha512 = (Sha512*)XMALLOC(sizeof(Sha512), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sha512 == NULL)
            return MEMORY_E;
    #endif

        if ((ret = wc_InitSha512(sha512)) != 0) {
            WOLFSSL_MSG("InitSha512 failed");
        }
        else if ((ret = wc_Sha512Update(sha512, data, len)) != 0) {
            WOLFSSL_MSG("Sha512Update failed");
        }
        else if ((ret = wc_Sha512Final(sha512, hash)) != 0) {
            WOLFSSL_MSG("Sha512Final failed");
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(sha512, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

        return ret;
    }

    #if defined(WOLFSSL_SHA384)
        int wc_Sha384Hash(const byte* data, word32 len, byte* hash)
        {
            int ret = 0;
        #ifdef WOLFSSL_SMALL_STACK
            Sha384* sha384;
        #else
            Sha384 sha384[1];
        #endif

        #ifdef WOLFSSL_SMALL_STACK
            sha384 = (Sha384*)XMALLOC(sizeof(Sha384), NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (sha384 == NULL)
                return MEMORY_E;
        #endif

            if ((ret = wc_InitSha384(sha384)) != 0) {
                WOLFSSL_MSG("InitSha384 failed");
            }
            else if ((ret = wc_Sha384Update(sha384, data, len)) != 0) {
                WOLFSSL_MSG("Sha384Update failed");
            }
            else if ((ret = wc_Sha384Final(sha384, hash)) != 0) {
                WOLFSSL_MSG("Sha384Final failed");
            }

        #ifdef WOLFSSL_SMALL_STACK
            XFREE(sha384, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif

            return ret;
        }
    #endif /* WOLFSSL_SHA384 */
#endif /* WOLFSSL_SHA512 */
