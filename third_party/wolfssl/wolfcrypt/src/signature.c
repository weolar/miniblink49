/* signature.c
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
#include <wolfssl/wolfcrypt/signature.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifndef NO_ASN
#include <wolfssl/wolfcrypt/asn.h>
#endif
#ifdef HAVE_ECC
#include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifndef NO_RSA
#include <wolfssl/wolfcrypt/rsa.h>
#endif

/* If ECC and RSA are disabled then disable signature wrapper */
#if (!defined(HAVE_ECC) || (defined(HAVE_ECC) && !defined(HAVE_ECC_SIGN) \
    && !defined(HAVE_ECC_VERIFY))) && defined(NO_RSA)
    #undef NO_SIG_WRAPPER
    #define NO_SIG_WRAPPER
#endif

/* Signature wrapper disabled check */
#ifndef NO_SIG_WRAPPER

#if !defined(NO_RSA) && !defined(NO_ASN)
static int wc_SignatureDerEncode(enum wc_HashType hash_type, byte** hash_data,
    word32* hash_len)
{
    int ret = wc_HashGetOID(hash_type);
    if (ret > 0) {
        int oid = ret;

        /* Allocate buffer for hash and max DER encoded */
        word32 digest_len = *hash_len + MAX_DER_DIGEST_SZ;
        byte *digest_buf = (byte*)XMALLOC(digest_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (digest_buf) {
            ret = wc_EncodeSignature(digest_buf, *hash_data, *hash_len, oid);
            if (ret > 0) {
                digest_len = ret;

                /* Replace hash with digest (DER encoding + hash) */
                XFREE(*hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                *hash_data = digest_buf;
                *hash_len = digest_len;
            }
            else {
                XFREE(digest_buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
        }
        else {
            ret = MEMORY_E;
        }
    }
    return ret;
}
#endif /* !NO_RSA && !NO_ASN */

int wc_SignatureGetSize(enum wc_SignatureType sig_type,
    const void* key, word32 key_len)
{
    int sig_len = BAD_FUNC_ARG;

    /* Suppress possible unused args if all signature types are disabled */
    (void)key;
    (void)key_len;

    switch(sig_type) {
        case WC_SIGNATURE_TYPE_ECC:
#ifdef HAVE_ECC
            /* Santity check that void* key is at least ecc_key in size */
            if (key_len >= sizeof(ecc_key)) {
                sig_len = wc_ecc_sig_size((ecc_key*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid ECC key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_RSA_W_ENC:
        case WC_SIGNATURE_TYPE_RSA:
#ifndef NO_RSA
            /* Santity check that void* key is at least RsaKey in size */
            if (key_len >= sizeof(RsaKey)) {
                sig_len = wc_RsaEncryptSize((RsaKey*)key);
            }
            else {
                WOLFSSL_MSG("wc_SignatureGetSize: Invalid RsaKey key size");
            }
#else
            sig_len = SIG_TYPE_E;
#endif
            break;

        case WC_SIGNATURE_TYPE_NONE:
        default:
            sig_len = BAD_FUNC_ARG;
            break;
    }
    return sig_len;
}

int wc_SignatureVerify(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    const byte* sig, word32 sig_len,
    const void* key, word32 key_len)
{
    int ret;
    word32 hash_len;
    byte *hash_data = NULL;

    /* Check arguments */
    if (data == NULL || data_len <= 0 || sig == NULL || sig_len <= 0 ||
        key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (1 to max is okay) */
    if ((int)sig_len > wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureVerify: Invalid hash type/len");
        return ret;
    }
    hash_len = ret;

    /* Allocate temporary buffer for hash data */
    hash_data = (byte*)XMALLOC(hash_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_data == NULL) {
        return MEMORY_E;
    }

    /* Perform hash of data */
    ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
    if(ret == 0) {
        /* Verify signature using hash as data */
        switch(sig_type) {
            case WC_SIGNATURE_TYPE_ECC:
            {
#if defined(HAVE_ECC) && defined(HAVE_ECC_VERIFY)
                int is_valid_sig = 0;

                /* Perform verification of signature using provided ECC key */
                do {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wc_AsyncWait(ret, &((ecc_key*)key)->asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                if (ret >= 0)
                    ret = wc_ecc_verify_hash(sig, sig_len, hash_data, hash_len,
                        &is_valid_sig, (ecc_key*)key);
                } while (ret == WC_PENDING_E);
                if (ret != 0 || is_valid_sig != 1) {
                    ret = SIG_VERIFY_E;
                }
#else
                ret = SIG_TYPE_E;
#endif
                break;
            }

            case WC_SIGNATURE_TYPE_RSA_W_ENC:
#if defined(NO_RSA) || defined(NO_ASN)
                ret = SIG_TYPE_E;
                break;
#else
                ret = wc_SignatureDerEncode(hash_type, &hash_data, &hash_len);
                /* Check for error */
                if (ret < 0) {
                    break;
                }
                /* Otherwise fall-through and perform normal RSA verify against updated
                 * DER encoding + hash */
#endif
                FALL_THROUGH;

            case WC_SIGNATURE_TYPE_RSA:
            {
#ifndef NO_RSA
                word32 plain_len = hash_len;
                byte *plain_data;

                /* Make sure the plain text output is at least key size */
                if (plain_len < sig_len) {
                    plain_len = sig_len;
                }
                plain_data = (byte*)XMALLOC(plain_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                if (plain_data) {
                    /* Perform verification of signature using provided RSA key */
                    do {
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        ret = wc_AsyncWait(ret, &((RsaKey*)key)->asyncDev,
                            WC_ASYNC_FLAG_CALL_AGAIN);
                    #endif
                    if (ret >= 0)
                        ret = wc_RsaSSL_Verify(sig, sig_len, plain_data,
                            plain_len, (RsaKey*)key);
                    } while (ret == WC_PENDING_E);
                    if (ret >= 0) {
                        if ((word32)ret == hash_len &&
                                XMEMCMP(plain_data, hash_data, hash_len) == 0) {
                            ret = 0; /* Success */
                        }
                        else {
                            WOLFSSL_MSG("RSA Signature Verify difference!");
                            ret = SIG_VERIFY_E;
                        }
                    }
                    XFREE(plain_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                }
                else {
                    ret = MEMORY_E;
                }
#else
                ret = SIG_TYPE_E;
#endif
                break;
            }

            case WC_SIGNATURE_TYPE_NONE:
            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (hash_data) {
        XFREE(hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

int wc_SignatureGenerate(
    enum wc_HashType hash_type, enum wc_SignatureType sig_type,
    const byte* data, word32 data_len,
    byte* sig, word32 *sig_len,
    const void* key, word32 key_len, WC_RNG* rng)
{
    int ret;
    word32 hash_len;
    byte *hash_data = NULL;

    /* Suppress possible unused arg if all signature types are disabled */
    (void)rng;

    /* Check arguments */
    if (data == NULL || data_len <= 0 || sig == NULL || sig_len == NULL ||
        *sig_len <= 0 || key == NULL || key_len <= 0) {
        return BAD_FUNC_ARG;
    }

    /* Validate signature len (needs to be at least max) */
    if ((int)*sig_len < wc_SignatureGetSize(sig_type, key, key_len)) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid sig type/len");
        return BAD_FUNC_ARG;
    }

    /* Validate hash size */
    ret = wc_HashGetDigestSize(hash_type);
    if (ret < 0) {
        WOLFSSL_MSG("wc_SignatureGenerate: Invalid hash type/len");
        return ret;
    }
    hash_len = ret;

    /* Allocate temporary buffer for hash data */
    hash_data = (byte*)XMALLOC(hash_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (hash_data == NULL) {
        return MEMORY_E;
    }

    /* Perform hash of data */
    ret = wc_Hash(hash_type, data, data_len, hash_data, hash_len);
    if (ret == 0) {
        /* Create signature using hash as data */
        switch(sig_type) {
            case WC_SIGNATURE_TYPE_ECC:
#if defined(HAVE_ECC) && defined(HAVE_ECC_SIGN)
                /* Create signature using provided ECC key */
                do {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wc_AsyncWait(ret, &((ecc_key*)key)->asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                if (ret >= 0)
                    ret = wc_ecc_sign_hash(hash_data, hash_len, sig, sig_len,
                        rng, (ecc_key*)key);
                } while (ret == WC_PENDING_E);
#else
                ret = SIG_TYPE_E;
#endif
                break;

            case WC_SIGNATURE_TYPE_RSA_W_ENC:
#if defined(NO_RSA) || defined(NO_ASN)
                ret = SIG_TYPE_E;
                break;
#else
                ret = wc_SignatureDerEncode(hash_type, &hash_data, &hash_len);
                /* Check for error */
                if (ret < 0) {
                    break;
                }
                /* Otherwise fall-through and perform normal RSA sign against updated
                 * DER encoding + hash */
#endif
                FALL_THROUGH;
            case WC_SIGNATURE_TYPE_RSA:
#ifndef NO_RSA
                /* Create signature using provided RSA key */
                do {
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wc_AsyncWait(ret, &((RsaKey*)key)->asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                    if (ret >= 0)
                        ret = wc_RsaSSL_Sign(hash_data, hash_len, sig, *sig_len,
                            (RsaKey*)key, rng);
                } while (ret == WC_PENDING_E);
                if (ret >= 0) {
                    *sig_len = ret;
                    ret = 0; /* Success */
                }
#else
                ret = SIG_TYPE_E;
#endif
                break;

            case WC_SIGNATURE_TYPE_NONE:
            default:
                ret = BAD_FUNC_ARG;
                break;
        }
    }

    if (hash_data) {
        XFREE(hash_data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

#endif /* NO_SIG_WRAPPER */
