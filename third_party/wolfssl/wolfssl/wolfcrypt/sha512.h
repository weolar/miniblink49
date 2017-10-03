/* sha512.h
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


#ifndef WOLF_CRYPT_SHA512_H
#define WOLF_CRYPT_SHA512_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_SHA512

/* for fips @wc_fips */
#ifdef HAVE_FIPS
    #define CYASSL_SHA512
    #if defined(WOLFSSL_SHA384)
        #define CYASSL_SHA384
    #endif
    #include <cyassl/ctaocrypt/sha512.h>
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
    SHA512              =   4,   /* hash type unique */
    SHA512_BLOCK_SIZE   = 128,
    SHA512_DIGEST_SIZE  =  64,
    SHA512_PAD_SIZE     = 112
};


/* Sha512 digest */
typedef struct Sha512 {
    word32  buffLen;   /* in bytes          */
    word64  loLen;     /* length in bytes   */
    word64  hiLen;     /* length in bytes   */
    word64  digest[SHA512_DIGEST_SIZE / sizeof(word64)];
    word64  buffer[SHA512_BLOCK_SIZE  / sizeof(word64)];
    void*   heap;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
} Sha512;

#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha512(Sha512*);
WOLFSSL_API int wc_InitSha512_ex(Sha512*, void*, int);
WOLFSSL_API int wc_Sha512Update(Sha512*, const byte*, word32);
WOLFSSL_API int wc_Sha512Final(Sha512*, byte*);
WOLFSSL_API void wc_Sha512Free(Sha512*);

WOLFSSL_API int wc_Sha512GetHash(Sha512*, byte*);
WOLFSSL_API int wc_Sha512Copy(Sha512* src, Sha512* dst);

#if defined(WOLFSSL_SHA384)

#ifndef HAVE_FIPS /* avoid redefinition of structs */
/* in bytes */
enum {
    SHA384              =   5,   /* hash type unique */
    SHA384_BLOCK_SIZE   =   SHA512_BLOCK_SIZE,
    SHA384_DIGEST_SIZE  =   48,
    SHA384_PAD_SIZE     =   SHA512_PAD_SIZE
};

typedef Sha512 Sha384;
#endif /* HAVE_FIPS */

WOLFSSL_API int wc_InitSha384(Sha384*);
WOLFSSL_API int wc_InitSha384_ex(Sha384*, void*, int);
WOLFSSL_API int wc_Sha384Update(Sha384*, const byte*, word32);
WOLFSSL_API int wc_Sha384Final(Sha384*, byte*);
WOLFSSL_API void wc_Sha384Free(Sha384*);

WOLFSSL_API int wc_Sha384GetHash(Sha384*, byte*);
WOLFSSL_API int wc_Sha384Copy(Sha384* src, Sha384* dst);

#endif /* WOLFSSL_SHA384 */

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_SHA512 */
#endif /* WOLF_CRYPT_SHA512_H */

