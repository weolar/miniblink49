/* wc_encrypt.h
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



#ifndef WOLF_CRYPT_ENCRYPT_H
#define WOLF_CRYPT_ENCRYPT_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef NO_AES
WOLFSSL_API int  wc_AesCbcEncryptWithKey(byte* out, const byte* in, word32 inSz,
                                         const byte* key, word32 keySz,
                                         const byte* iv);
WOLFSSL_API int  wc_AesCbcDecryptWithKey(byte* out, const byte* in, word32 inSz,
                                         const byte* key, word32 keySz,
                                         const byte* iv);
#endif /* !NO_AES */


#ifndef NO_DES3
WOLFSSL_API int  wc_Des_CbcDecryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);
WOLFSSL_API int  wc_Des_CbcEncryptWithKey(byte* out,
                                          const byte* in, word32 sz,
                                          const byte* key, const byte* iv);
WOLFSSL_API int  wc_Des3_CbcEncryptWithKey(byte* out,
                                           const byte* in, word32 sz,
                                           const byte* key, const byte* iv);
WOLFSSL_API int  wc_Des3_CbcDecryptWithKey(byte* out,
                                           const byte* in, word32 sz,
                                           const byte* key, const byte* iv);
#endif /* !NO_DES3 */

#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLF_CRYPT_ENCRYPT_H */

