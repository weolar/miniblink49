/* aes.h
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



/*  aes.h defines mini des openssl compatibility layer
 *
 */


#ifndef WOLFSSL_AES_H_
#define WOLFSSL_AES_H_

#include <wolfssl/wolfcrypt/settings.h>

#ifndef NO_AES
#ifdef WOLFSSL_AES_DIRECT

#ifdef __cplusplus
    extern "C" {
#endif


typedef Aes AES_KEY;

WOLFSSL_API void wolfSSL_AES_set_encrypt_key
    (const unsigned char *, const int bits, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_set_decrypt_key
    (const unsigned char *, const int bits, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_encrypt
    (const unsigned char* input, unsigned char* output, AES_KEY *);
WOLFSSL_API void wolfSSL_AES_decrypt
    (const unsigned char* input, unsigned char* output, AES_KEY *);

#define AES_set_encrypt_key wolfSSL_AES_set_encrypt_key
#define AES_set_decrypt_key wolfSSL_AES_set_decrypt_key
#define AES_encrypt         wolfSSL_AES_encrypt
#define AES_decrypt         wolfSSL_AES_decrypt

#define wolfSSL_AES_set_encrypt_key(key, bits, aes) \
    wc_AesSetKey(aes, key, ((bits)/8), NULL, AES_ENCRYPTION)
#define wolfSSL_AES_set_decrypt_key(key, bits, aes) \
    wc_AesSetKey(aes, key, ((bits)/8), NULL, AES_DECRYPTION)

#define wolfSSL_AES_encrypt(in, out, aes) wc_AesEncryptDirect(aes, out, in)
#define wolfSSL_AES_decrypt(in, out, aes) wc_AesDecryptDirect(aes, out, in)

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_AES_DIRECT */
#endif /* NO_AES */

#endif /* WOLFSSL_DES_H_ */
