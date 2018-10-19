/* wc_pkcs11.h
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL. (formerly known as CyaSSL)
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef _WOLFPKCS11_H_
#define _WOLFPKCS11_H_

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_PKCS11

#include <wolfssl/wolfcrypt/cryptodev.h>
#include <wolfssl/wolfcrypt/pkcs11.h>

typedef struct Pkcs11Dev {
    void*             dlHandle;
    CK_FUNCTION_LIST* func;                    /* Array of functions */
} Pkcs11Dev;

typedef struct Pkcs11Token {
    CK_FUNCTION_LIST* func;
    CK_SLOT_ID        slotId;
    CK_SESSION_HANDLE handle;
    CK_UTF8CHAR_PTR   userPin;
    CK_ULONG          userPinSz;
} Pkcs11Token;

typedef struct Pkcs11Session {
    CK_FUNCTION_LIST* func;
    CK_SLOT_ID        slotId;
    CK_SESSION_HANDLE handle;
} Pkcs11Session;

#ifdef __cplusplus
    extern "C" {
#endif

/* Types of keys that can be stored. */
enum Pkcs11KeyType {
    PKCS11_KEY_TYPE_AES_GCM,
    PKCS11_KEY_TYPE_RSA,
    PKCS11_KEY_TYPE_EC,
};


WOLFSSL_API int wc_Pkcs11_Initialize(Pkcs11Dev* dev, const char* library);
WOLFSSL_API void wc_Pkcs11_Finalize(Pkcs11Dev* dev);

WOLFSSL_API int wc_Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev,
    int slotId, const char* tokenName, const unsigned char *userPin,
    int userPinSz);
WOLFSSL_API void wc_Pkcs11Token_Final(Pkcs11Token* token);
WOLFSSL_API int wc_Pkcs11Token_Open(Pkcs11Token* token, int readWrite);
WOLFSSL_API void wc_Pkcs11Token_Close(Pkcs11Token* token);

WOLFSSL_API int wc_Pkcs11StoreKey(Pkcs11Token* token, int type, int clear,
    void* key);

WOLFSSL_API int wc_Pkcs11_CryptoDevCb(int devId, wc_CryptoInfo* info,
    void* ctx);

#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_PKCS11 */

#endif /* _WOLFPKCS11_H_ */
