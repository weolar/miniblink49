/* cmac.h
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


#ifndef WOLF_CRYPT_CMAC_H
#define WOLF_CRYPT_CMAC_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/aes.h>

#if !defined(NO_AES) && defined(WOLFSSL_CMAC)

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct Cmac {
    Aes aes;
    byte buffer[AES_BLOCK_SIZE]; /* partially stored block */
    byte digest[AES_BLOCK_SIZE]; /* running digest */
    byte k1[AES_BLOCK_SIZE];
    byte k2[AES_BLOCK_SIZE];
    word32 bufferSz;
    word32 totalSz;
} Cmac;


typedef enum CmacType {
    WC_CMAC_AES = 1
} CmacType;


WOLFSSL_API
int wc_InitCmac(Cmac* cmac,
                const byte* key, word32 keySz,
                int type, void* unused);
WOLFSSL_API
int wc_CmacUpdate(Cmac* cmac,
                  const byte* in, word32 inSz);
WOLFSSL_API
int wc_CmacFinal(Cmac* cmac,
                 byte* out, word32* outSz);

WOLFSSL_API
int wc_AesCmacGenerate(byte* out, word32* outSz,
                       const byte* in, word32 inSz,
                       const byte* key, word32 keySz);

WOLFSSL_API
int wc_AesCmacVerify(const byte* check, word32 checkSz,
                     const byte* in, word32 inSz,
                     const byte* key, word32 keySz);

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* NO_AES && WOLFSSL_CMAC */
#endif /* WOLF_CRYPT_CMAC_H */

