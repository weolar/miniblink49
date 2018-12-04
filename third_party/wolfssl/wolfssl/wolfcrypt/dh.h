/* dh.h
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
    \file wolfssl/wolfcrypt/dh.h
*/

#ifndef WOLF_CRYPT_DH_H
#define WOLF_CRYPT_DH_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_DH

#include <wolfssl/wolfcrypt/integer.h>
#include <wolfssl/wolfcrypt/random.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif
typedef struct DhParams {
    const byte* p;
    word32      p_len;
    const byte* g;
    word32      g_len;
} DhParams;

/* Diffie-Hellman Key */
typedef struct DhKey {
    mp_int p, g, q;                         /* group parameters  */
    void* heap;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif
} DhKey;


#ifdef HAVE_FFDHE_2048
WOLFSSL_API const DhParams* wc_Dh_ffdhe2048_Get(void);
#endif
#ifdef HAVE_FFDHE_3072
WOLFSSL_API const DhParams* wc_Dh_ffdhe3072_Get(void);
#endif
#ifdef HAVE_FFDHE_4096
WOLFSSL_API const DhParams* wc_Dh_ffdhe4096_Get(void);
#endif
#ifdef HAVE_FFDHE_6144
WOLFSSL_API const DhParams* wc_Dh_ffdhe6144_Get(void);
#endif
#ifdef HAVE_FFDHE_8192
WOLFSSL_API const DhParams* wc_Dh_ffdhe8192_Get(void);
#endif

WOLFSSL_API int wc_InitDhKey(DhKey* key);
WOLFSSL_API int wc_InitDhKey_ex(DhKey* key, void* heap, int devId);
WOLFSSL_API void wc_FreeDhKey(DhKey* key);

WOLFSSL_API int wc_DhGenerateKeyPair(DhKey* key, WC_RNG* rng, byte* priv,
                                 word32* privSz, byte* pub, word32* pubSz);
WOLFSSL_API int wc_DhAgree(DhKey* key, byte* agree, word32* agreeSz,
                       const byte* priv, word32 privSz, const byte* otherPub,
                       word32 pubSz);

WOLFSSL_API int wc_DhKeyDecode(const byte* input, word32* inOutIdx, DhKey* key,
                           word32);
WOLFSSL_API int wc_DhSetKey(DhKey* key, const byte* p, word32 pSz, const byte* g,
                        word32 gSz);
WOLFSSL_API int wc_DhSetKey_ex(DhKey* key, const byte* p, word32 pSz,
                        const byte* g, word32 gSz, const byte* q, word32 qSz);
WOLFSSL_API int wc_DhParamsLoad(const byte* input, word32 inSz, byte* p,
                            word32* pInOutSz, byte* g, word32* gInOutSz);
WOLFSSL_API int wc_DhCheckPubKey(DhKey* key, const byte* pub, word32 pubSz);
WOLFSSL_API int wc_DhCheckPubKey_ex(DhKey* key, const byte* pub, word32 pubSz,
                            const byte* prime, word32 primeSz);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* NO_DH */
#endif /* WOLF_CRYPT_DH_H */

