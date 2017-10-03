/* sha3.h
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


#ifndef WOLF_CRYPT_SHA3_H
#define WOLF_CRYPT_SHA3_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_SHA3

#ifdef HAVE_FIPS
    /* for fips @wc_fips */
    #include <cyassl/ctaocrypt/sha3.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef HAVE_FIPS /* avoid redefinition of structs */

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

/* in bytes */
enum {
    SHA3_224              = 10,   /* hash type unique */
    SHA3_224_DIGEST_SIZE  = 28,
    SHA3_224_COUNT        = 18,

    SHA3_256              = 11,   /* hash type unique */
    SHA3_256_DIGEST_SIZE  = 32,
    SHA3_256_COUNT        = 17,

    SHA3_384              = 12,   /* hash type unique */
    SHA3_384_DIGEST_SIZE  = 48,
    SHA3_384_COUNT        = 13,

    SHA3_512              = 13,   /* hash type unique */
    SHA3_512_DIGEST_SIZE  = 64,
    SHA3_512_COUNT        =  9
};


#ifdef WOLFSSL_XILINX_CRYPT
    #include "wolfssl/wolfcrypt/port/xilinx/xil-sha3.h"
#else
/* Sha3 digest */
typedef struct Sha3 {
    /* State data that is processed for each block. */
    word64 s[25];
    /* Unprocessed message data. */
    byte   t[200];
    /* Index into unprocessed data to place next message byte. */
    byte   i;

    void*  heap;

#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} Sha3;
#endif
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha3_224(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_224_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_224_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_224_Free(Sha3*);
WOLFSSL_API int wc_Sha3_224_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_224_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_256(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_256_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_256_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_256_Free(Sha3*);
WOLFSSL_API int wc_Sha3_256_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_256_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_384(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_384_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_384_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_384_Free(Sha3*);
WOLFSSL_API int wc_Sha3_384_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_384_Copy(Sha3* src, Sha3* dst);

WOLFSSL_API int wc_InitSha3_512(Sha3*, void*, int);
WOLFSSL_API int wc_Sha3_512_Update(Sha3*, const byte*, word32);
WOLFSSL_API int wc_Sha3_512_Final(Sha3*, byte*);
WOLFSSL_API void wc_Sha3_512_Free(Sha3*);
WOLFSSL_API int wc_Sha3_512_GetHash(Sha3*, byte*);
WOLFSSL_API int wc_Sha3_512_Copy(Sha3* src, Sha3* dst);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA3 */
#endif /* WOLF_CRYPT_SHA3_H */

