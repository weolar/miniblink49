/* pkcs7.h
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


#ifndef WOLF_CRYPT_PKCS7_H
#define WOLF_CRYPT_PKCS7_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef HAVE_PKCS7

#ifndef NO_ASN
    #include <wolfssl/wolfcrypt/asn.h>
#endif
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/random.h>
#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
#endif
#ifndef NO_DES3
    #include <wolfssl/wolfcrypt/des3.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

/* PKCS#7 content types, ref RFC 2315 (Section 14) */
enum PKCS7_TYPES {
    PKCS7_MSG                 = 650,   /* 1.2.840.113549.1.7   */
    DATA                      = 651,   /* 1.2.840.113549.1.7.1 */
    SIGNED_DATA               = 652,   /* 1.2.840.113549.1.7.2 */
    ENVELOPED_DATA            = 653,   /* 1.2.840.113549.1.7.3 */
    SIGNED_AND_ENVELOPED_DATA = 654,   /* 1.2.840.113549.1.7.4 */
    DIGESTED_DATA             = 655,   /* 1.2.840.113549.1.7.5 */
    ENCRYPTED_DATA            = 656    /* 1.2.840.113549.1.7.6 */
};

enum Pkcs7_Misc {
    PKCS7_NONCE_SZ        = 16,
    MAX_ENCRYPTED_KEY_SZ  = 512,    /* max enc. key size, RSA <= 4096 */
    MAX_CONTENT_KEY_LEN   = 32,     /* highest current cipher is AES-256-CBC */
    MAX_CONTENT_IV_SIZE   = 16,     /* highest current is AES128 */
#ifndef NO_AES
    MAX_CONTENT_BLOCK_LEN = AES_BLOCK_SIZE,
#else
    MAX_CONTENT_BLOCK_LEN = DES_BLOCK_SIZE,
#endif
    MAX_RECIP_SZ          = MAX_VERSION_SZ +
                            MAX_SEQ_SZ + ASN_NAME_MAX + MAX_SN_SZ +
                            MAX_SEQ_SZ + MAX_ALGO_SZ + 1 + MAX_ENCRYPTED_KEY_SZ
};


typedef struct PKCS7Attrib {
    byte* oid;
    word32 oidSz;
    byte* value;
    word32 valueSz;
} PKCS7Attrib;


typedef struct PKCS7DecodedAttrib {
    byte* oid;
    word32 oidSz;
    byte* value;
    word32 valueSz;
    struct PKCS7DecodedAttrib* next;
} PKCS7DecodedAttrib;


typedef struct PKCS7 {
    byte* content;                /* inner content, not owner             */
    word32 contentSz;             /* content size                         */
    int contentOID;               /* PKCS#7 content type OID sum          */

    WC_RNG* rng;

    int hashOID;
    int encryptOID;               /* key encryption algorithm OID         */
    int keyWrapOID;               /* key wrap algorithm OID               */
    int keyAgreeOID;              /* key agreement algorithm OID          */

    void*  heap;                  /* heap hint for dynamic memory         */
    byte*  singleCert;            /* recipient cert, DER, not owner       */
    word32 singleCertSz;          /* size of recipient cert buffer, bytes */
    byte issuerHash[KEYID_SIZE];  /* hash of all alt Names                */
    byte*  issuer;                /* issuer name of singleCert            */
    word32 issuerSz;              /* length of issuer name                */
    byte issuerSn[MAX_SN_SZ];     /* singleCert's serial number           */
    word32 issuerSnSz;            /* length of serial number              */

    byte publicKey[512];
    word32 publicKeySz;
    word32 publicKeyOID;          /* key OID (RSAk, ECDSAk, etc) */
    byte*  privateKey;            /* private key, DER, not owner          */
    word32 privateKeySz;          /* size of private key buffer, bytes    */

    PKCS7Attrib* signedAttribs;
    word32 signedAttribsSz;

    /* Enveloped-data optional ukm, not owner */
    byte*  ukm;
    word32 ukmSz;

    /* Encrypted-data Content Type */
    byte*        encryptionKey;         /* block cipher encryption key */
    word32       encryptionKeySz;       /* size of key buffer, bytes */
    PKCS7Attrib* unprotectedAttribs;    /* optional */
    word32       unprotectedAttribsSz;
    PKCS7DecodedAttrib* decodedAttrib;  /* linked list of decoded attribs */
} PKCS7;


WOLFSSL_API int  wc_PKCS7_Init(PKCS7* pkcs7, void* heap, int devId);
WOLFSSL_API int  wc_PKCS7_InitWithCert(PKCS7* pkcs7, byte* cert, word32 certSz);
WOLFSSL_API void wc_PKCS7_Free(PKCS7* pkcs7);
WOLFSSL_API int  wc_PKCS7_EncodeData(PKCS7* pkcs7, byte* output,
                                       word32 outputSz);
WOLFSSL_API int  wc_PKCS7_EncodeSignedData(PKCS7* pkcs7,
                                       byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_VerifySignedData(PKCS7* pkcs7,
                                       byte* pkiMsg, word32 pkiMsgSz);
WOLFSSL_API int  wc_PKCS7_EncodeEnvelopedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_DecodeEnvelopedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);
WOLFSSL_API int  wc_PKCS7_EncodeEncryptedData(PKCS7* pkcs7,
                                          byte* output, word32 outputSz);
WOLFSSL_API int  wc_PKCS7_DecodeEncryptedData(PKCS7* pkcs7, byte* pkiMsg,
                                          word32 pkiMsgSz, byte* output,
                                          word32 outputSz);
#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* HAVE_PKCS7 */
#endif /* WOLF_CRYPT_PKCS7_H */

