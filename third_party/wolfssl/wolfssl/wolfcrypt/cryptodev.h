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

/* Crypto Information Structure for callbacks */
typedef struct wc_CryptoInfo {
    int algo_type; /* enum wc_AlgoType */
    struct {
        int type; /* enum wc_PkType */
        union {
        #ifndef NO_RSA
            struct {
                const byte* in;
                word32 inLen;
                byte* out;
                word32* outLen;
                int type;
                RsaKey* key;
                WC_RNG* rng;
            } rsa;
        #endif
        #ifdef HAVE_ECC
            struct {
                ecc_key* private_key;
                ecc_key* public_key;
                byte* out;
                word32* outlen;
            } ecdh;
            struct {
                const byte* in;
                word32 inlen;
                byte* out;
                word32 *outlen;
                WC_RNG* rng;
                ecc_key* key;
            } eccsign;
            struct {
                const byte* sig;
                word32 siglen;
                const byte* hash;
                word32 hashlen;
                int* res;
                ecc_key* key;
            } eccverify;
        #endif
        };
    } pk;
} wc_CryptoInfo;

typedef int (*CryptoDevCallbackFunc)(int devId, wc_CryptoInfo* info, void* ctx);

WOLFSSL_LOCAL void wc_CryptoDev_Init(void);

WOLFSSL_API int  wc_CryptoDev_RegisterDevice(int devId, CryptoDevCallbackFunc cb, void* ctx);
WOLFSSL_API void wc_CryptoDev_UnRegisterDevice(int devId);


#ifndef NO_RSA
WOLFSSL_LOCAL int wc_CryptoDev_Rsa(const byte* in, word32 inLen, byte* out,
    word32* outLen, int type, RsaKey* key, WC_RNG* rng);
#endif /* !NO_RSA */

#ifdef HAVE_ECC
WOLFSSL_LOCAL int wc_CryptoDev_Ecdh(ecc_key* private_key, ecc_key* public_key,
    byte* out, word32* outlen);

WOLFSSL_LOCAL int wc_CryptoDev_EccSign(const byte* in, word32 inlen, byte* out,
    word32 *outlen, WC_RNG* rng, ecc_key* key);

WOLFSSL_LOCAL int wc_CryptoDev_EccVerify(const byte* sig, word32 siglen,
    const byte* hash, word32 hashlen, int* res, ecc_key* key);
#endif /* HAVE_ECC */

#endif /* WOLF_CRYPTO_DEV */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* _WOLF_CRYPTO_DEV_H_ */
