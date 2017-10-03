/* blake2.h
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




#ifndef WOLF_CRYPT_BLAKE2_H
#define WOLF_CRYPT_BLAKE2_H

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_BLAKE2

#include <wolfssl/wolfcrypt/blake2-int.h>

/* call old functions if using fips for the sake of hmac @wc_fips */
#ifdef HAVE_FIPS
    /* Since hmac can call blake functions provide original calls */
    #define wc_InitBlake2b   InitBlake2b
    #define wc_Blake2bUpdate Blake2bUpdate
    #define wc_Blake2bFinal  Blake2bFinal
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* in bytes, variable digest size up to 512 bits (64 bytes) */
enum {
    BLAKE2B_ID  = 7,   /* hash type unique */
    BLAKE2B_256 = 32   /* 256 bit type, SSL default */
};


/* BLAKE2b digest */
typedef struct Blake2b {
    blake2b_state S[1];         /* our state */
    word32        digestSz;     /* digest size used on init */
} Blake2b;


WOLFSSL_API int wc_InitBlake2b(Blake2b*, word32);
WOLFSSL_API int wc_Blake2bUpdate(Blake2b*, const byte*, word32);
WOLFSSL_API int wc_Blake2bFinal(Blake2b*, byte*, word32);



#ifdef __cplusplus
    }
#endif

#endif  /* HAVE_BLAKE2 */
#endif  /* WOLF_CRYPT_BLAKE2_H */

