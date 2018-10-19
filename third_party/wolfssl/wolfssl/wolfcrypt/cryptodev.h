/* cryptodev.h
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _WOLF_CRYPTO_DEV_H_
#define _WOLF_CRYPTO_DEV_H_

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLF_CRYPTO_DEV

#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif

/* Crypto Information Structure for callbacks */
typedef struct wc_CryptoInfo {
    int algo_type; /* enum wc_AlgoType */
    struct {
        int type; /* enum wc_PkType */
        union {
        #ifndef NO_RSA
            struct {
                const byte* in;
                word32      inLen;
                byte*       out;
                word32*     outLen;
                int         type;
                RsaKey*     key;
                WC_RNG*     rng;
            } rsa;
        #ifdef WOLFSSL_KEY_GEN
            struct {
                RsaKey* key;
                int     size;
                long    e;
                WC_RNG* rng;
            } rsakg;
        #endif
        #endif
        #ifdef HAVE_ECC
            struct {
                WC_RNG*  rng;
                int      size;
                ecc_key* key;
                int      curveId;
            } eckg;
            struct {
                ecc_key* private_key;
                ecc_key* public_key;
                byte*    out;
                word32*  outlen;
            } ecdh;
            struct {
                const byte* in;
                word32      inlen;
                byte*       out;
                word32*     outlen;
                WC_RNG*     rng;
                ecc_key*    key;
            } eccsign;
            struct {
                const byte* sig;
                word32      siglen;
                const byte* hash;
                word32      hashlen;
                int*        res;
                ecc_key*    key;
            } eccverify;
        #endif
        };
    } pk;
    struct {
        int type; /* enum wc_CipherType */
        int enc;
        union {
        #if !defined(NO_AES) && defined(HAVE_AESGCM)
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* iv;
                word32      ivSz;
                byte*       authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesgcm_enc;
            struct {
                Aes*        aes;
                byte*       out;
                const byte* in;
                word32      sz;
                const byte* iv;
                word32      ivSz;
                const byte* authTag;
                word32      authTagSz;
                const byte* authIn;
                word32      authInSz;
            } aesgcm_dec;
        #endif
        };
    } cipher;
} wc_CryptoInfo;

typedef int (*CryptoDevCallbackFunc)(int devId, wc_CryptoInfo* info, void* ctx);

WOLFSSL_LOCAL void wc_CryptoDev_Init(void);

WOLFSSL_API int  wc_CryptoDev_RegisterDevice(int devId, CryptoDevCallbackFunc cb, void* ctx);
WOLFSSL_API void wc_CryptoDev_UnRegisterDevice(int devId);


#ifndef NO_RSA
WOLFSSL_LOCAL int wc_CryptoDev_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng);

#ifdef WOLFSSL_KEY_GEN
WOLFSSL_LOCAL int wc_CryptoDev_MakeRsaKey(RsaKey* key, int size, long e,
    WC_RNG* rng);
#endif /* WOLFSSL_KEY_GEN */
#endif /* !NO_RSA */

#ifdef HAVE_ECC
WOLFSSL_LOCAL int wc_CryptoDev_MakeEccKey(WC_RNG* rng, int keySize,
    ecc_key* key, int curveId);

WOLFSSL_LOCAL int wc_CryptoDev_Ecdh(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen);

WOLFSSL_LOCAL int wc_CryptoDev_EccSign(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key);

WOLFSSL_LOCAL int wc_CryptoDev_EccVerify(const byte* sig, word32 siglen,
    const byte* hash, word32 hashlen, int* res, ecc_key* key);
#endif /* HAVE_ECC */

#if !defined(NO_AES) && defined(HAVE_AESGCM)

WOLFSSL_LOCAL int wc_CryptoDev_AesGcmEncrypt(Aes* aes, byte* out,
     const byte* in, word32 sz, const byte* iv, word32 ivSz,
     byte* authTag, word32 authTagSz, const byte* authIn, word32 authInSz);

WOLFSSL_LOCAL int wc_CryptoDev_AesGcmDecrypt(Aes* aes, byte* out,
     const byte* in, word32 sz, const byte* iv, word32 ivSz,
     const byte* authTag, word32 authTagSz,
     const byte* authIn, word32 authInSz);

#endif /* !NO_AES && HAVE_AESGCM */

WOLFSSL_LOCAL int wc_CryptoDev_Sha256Hash(const byte* data, word32 len, byte* hash);

#endif /* WOLF_CRYPTO_DEV */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* _WOLF_CRYPTO_DEV_H_ */
