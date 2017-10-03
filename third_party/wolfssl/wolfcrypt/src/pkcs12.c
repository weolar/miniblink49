/* pkcs12.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if !defined(NO_ASN) && !defined(NO_PWDBASED)

#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif
#include <wolfssl/wolfcrypt/pkcs12.h>
#include <wolfssl/wolfcrypt/pwdbased.h>


#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

enum {
    WC_PKCS12_KeyBag = 667,
    WC_PKCS12_ShroudedKeyBag = 668,
    WC_PKCS12_CertBag = 669,
    WC_PKCS12_CertBag_Type1 = 675,
    WC_PKCS12_CrlBag = 670,
    WC_PKCS12_SecretBag = 671,
    WC_PKCS12_SafeContentsBag = 672,
    WC_PKCS12_DATA = 651,
    WC_PKCS12_ENCRYPTED_DATA = 656,
};

typedef struct ContentInfo {
    byte* data;
    struct ContentInfo* next;
    word32 encC;  /* encryptedContent */
    word32 dataSz;
    int type; /* DATA / encrypted / envelpoed */
} ContentInfo;


typedef struct AuthenticatedSafe {
    ContentInfo* CI;
    byte* data; /* T contents.... */
    word32 oid; /* encrypted or not */
    word32 numCI; /* number of Content Info structs */
    word32 dataSz;
} AuthenticatedSafe;


typedef struct MacData {
    byte* digest;
    byte* salt;
    word32 oid;
    word32 digestSz;
    word32 saltSz;
    int itt; /* number of itterations when creating HMAC key */
} MacData;


struct WC_PKCS12 {
    void* heap;
    AuthenticatedSafe* safe;
    MacData* signData;
    word32 oid; /* DATA / Enveloped DATA ... */
};


/* for friendlyName, localKeyId .... */
typedef struct WC_PKCS12_ATTRIBUTE {
    byte* data;
    word32 oid;
    word32 dataSz;
} WC_PKCS12_ATTRIBUTE;


WC_PKCS12* wc_PKCS12_new(void)
{
    WC_PKCS12* pkcs12 = (WC_PKCS12*)XMALLOC(sizeof(WC_PKCS12),
                                                      NULL, DYNAMIC_TYPE_PKCS);
    if (pkcs12 == NULL) {
        WOLFSSL_MSG("Memory issue when creating WC_PKCS12 struct");
        return NULL;
    }

    XMEMSET(pkcs12, 0, sizeof(WC_PKCS12));

    return pkcs12;
}


static void freeSafe(AuthenticatedSafe* safe, void* heap)
{
    int i;

    if (safe == NULL) {
        return;
    }

    /* free content info structs */
    for (i = safe->numCI; i > 0; i--) {
        ContentInfo* ci = safe->CI;
        safe->CI = ci->next;
        XFREE(ci, heap, DYNAMIC_TYPE_PKCS);
    }
    if (safe->data != NULL) {
        XFREE(safe->data, heap, DYNAMIC_TYPE_PKCS);
    }
    XFREE(safe, heap, DYNAMIC_TYPE_PKCS);

    (void)heap;
}


void wc_PKCS12_free(WC_PKCS12* pkcs12)
{
    void* heap;

    /* if null pointer is passed in do nothing */
    if (pkcs12 == NULL) {
        WOLFSSL_MSG("Trying to free null WC_PKCS12 object");
        return;
    }

    heap = pkcs12->heap;
    if (pkcs12->safe != NULL) {
    	freeSafe(pkcs12->safe, heap);
    }

    /* free mac data */
    if (pkcs12->signData != NULL) {
        if (pkcs12->signData->digest != NULL) {
            XFREE(pkcs12->signData->digest, heap, DYNAMIC_TYPE_DIGEST);
            pkcs12->signData->digest = NULL;
        }
        if (pkcs12->signData->salt != NULL) {
            XFREE(pkcs12->signData->salt, heap, DYNAMIC_TYPE_SALT);
            pkcs12->signData->salt = NULL;
        }
        XFREE(pkcs12->signData, heap, DYNAMIC_TYPE_PKCS);
        pkcs12->signData = NULL;
    }

    XFREE(pkcs12, NULL, DYNAMIC_TYPE_PKCS);
    pkcs12 = NULL;
}


static int GetSafeContent(WC_PKCS12* pkcs12, const byte* input,
                          word32* idx, int maxIdx)
{
    AuthenticatedSafe* safe;
    word32 oid;
    word32 localIdx = *idx;
    int ret;
    int size = 0;

    safe = (AuthenticatedSafe*)XMALLOC(sizeof(AuthenticatedSafe), pkcs12->heap,
                                       DYNAMIC_TYPE_PKCS);
    if (safe == NULL) {
        return MEMORY_E;
    }
    XMEMSET(safe, 0, sizeof(AuthenticatedSafe));

    ret = GetObjectId(input, &localIdx, &oid, oidIgnoreType, maxIdx);
    if (ret < 0) {
        WOLFSSL_LEAVE("Get object id failed", ret);
        freeSafe(safe, pkcs12->heap);
        return ASN_PARSE_E;
    }

    safe->oid = oid;
    /* check tag, length */
    if (input[localIdx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
        WOLFSSL_MSG("Unexpected tag in PKCS12 DER");
        freeSafe(safe, pkcs12->heap);
        return ASN_PARSE_E;
    }
    if ((ret = GetLength(input, &localIdx, &size, maxIdx)) <= 0) {
        freeSafe(safe, pkcs12->heap);
        return ret;
    }

    switch (oid) {
        case WC_PKCS12_ENCRYPTED_DATA:
            WOLFSSL_MSG("Found PKCS12 OBJECT: ENCRYPTED DATA\n");
            break;

        case WC_PKCS12_DATA:
            WOLFSSL_MSG("Found PKCS12 OBJECT: DATA");
            /* get octets holding contents */
            if (input[localIdx++] != ASN_OCTET_STRING) {
                WOLFSSL_MSG("Wrong tag with content PKCS12 type DATA");
                freeSafe(safe, pkcs12->heap);
                return ASN_PARSE_E;
            }
            if ((ret = GetLength(input, &localIdx, &size, maxIdx)) <= 0) {
                freeSafe(safe, pkcs12->heap);
                return ret;
            }

            break;
    }

    safe->dataSz = size;
    safe->data = (byte*)XMALLOC(size, pkcs12->heap, DYNAMIC_TYPE_PKCS);
    if (safe->data == NULL) {
        freeSafe(safe, pkcs12->heap);
        return MEMORY_E;
    }
    XMEMCPY(safe->data, input + localIdx, size);
    *idx = localIdx;

    /* an instance of AuthenticatedSafe is created from
     * ContentInfo's strung together in a SEQUENCE. Here we itterate
     * through the ContentInfo's and add them to our
     * AuthenticatedSafe struct */
    localIdx = 0;
    input = safe->data;
    {
        int CISz;
        ret = GetSequence(input, &localIdx, &CISz, safe->dataSz);
        if (ret < 0) {
            freeSafe(safe, pkcs12->heap);
            return ASN_PARSE_E;
        }
        CISz += localIdx;
        while ((int)localIdx < CISz) {
            int curSz = 0;
            word32 curIdx;
            ContentInfo* ci = NULL;

        #ifdef WOLFSSL_DEBUG_PKCS12
            printf("\t\tlooking for Content Info.... ");
        #endif

            if ((ret = GetSequence(input, &localIdx, &curSz, safe->dataSz))
                                                                          < 0) {
                freeSafe(safe, pkcs12->heap);
                return ret;
            }

            if (curSz > CISz) {
                /* subset should not be larger than universe */
                freeSafe(safe, pkcs12->heap);
                return ASN_PARSE_E;
            }

            curIdx = localIdx;
            if ((ret = GetObjectId(input, &localIdx, &oid, oidIgnoreType,
                                                           safe->dataSz)) < 0) {
                WOLFSSL_LEAVE("Get object id failed", ret);
                freeSafe(safe, pkcs12->heap);
                return ret;
            }

            /* create new content info struct ... possible OID sanity check? */
            ci = (ContentInfo*)XMALLOC(sizeof(ContentInfo), pkcs12->heap,
                                       DYNAMIC_TYPE_PKCS);
            if (ci == NULL) {
                freeSafe(safe, pkcs12->heap);
                return MEMORY_E;
            }

            ci->type   = oid;
            ci->dataSz = curSz - (localIdx-curIdx);
            ci->data   = (byte*)input + localIdx;
            localIdx  += ci->dataSz;

        #ifdef WOLFSSL_DEBUG_PKCS12
            switch (oid) {
                case WC_PKCS12_ENCRYPTED_DATA:
                    printf("CONTENT INFO: ENCRYPTED DATA, size = %d\n", ci->dataSz);
                    break;

                case WC_PKCS12_DATA:
                    printf("CONTENT INFO: DATA, size = %d\n", ci->dataSz);
                    break;
                default:
                    printf("CONTENT INFO: UNKNOWN, size = %d\n", ci->dataSz);
            }
        #endif

            /* insert to head of list */
            ci->next = safe->CI;
            safe->CI = ci;
            safe->numCI += 1;
        }
    }

    pkcs12->safe = safe;
    *idx += localIdx;

    return ret;
}


/* optional mac data */
static int GetSignData(WC_PKCS12* pkcs12, const byte* mem, word32* idx,
                       word32 totalSz)
{
    MacData* mac;
    word32 curIdx = *idx;
    word32 oid = 0;
    int size, ret;

    /* Digest Info : Sequence
     *      DigestAlgorithmIdentifier
     *      Digest
     */
    if ((ret = GetSequence(mem, &curIdx, &size, totalSz)) <= 0) {
        WOLFSSL_MSG("Failed to get PKCS12 sequence");
        return ret;
    }

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("\t\tSEQUENCE: DigestInfo size = %d\n", size);
#endif

    mac = (MacData*)XMALLOC(sizeof(MacData), pkcs12->heap, DYNAMIC_TYPE_PKCS);
    if (mac == NULL) {
        return MEMORY_E;
    }
    XMEMSET(mac, 0, sizeof(MacData));

    /* DigestAlgorithmIdentifier */
    if ((ret = GetAlgoId(mem, &curIdx, &oid, oidIgnoreType, totalSz)) < 0) {
        WOLFSSL_MSG("Failed to get PKCS12 sequence");
        XFREE(mac, pkcs12->heap, DYNAMIC_TYPE_PKCS);
        return ret;
    }
    mac->oid = oid;

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("\t\tALGO ID = %d\n", oid);
#endif

    /* Digest: should be octet type holding digest */
    if (mem[curIdx++] != ASN_OCTET_STRING) {
        WOLFSSL_MSG("Failed to get digest");
        XFREE(mac, pkcs12->heap, DYNAMIC_TYPE_PKCS);
        return ASN_PARSE_E;
    }

    if ((ret = GetLength(mem, &curIdx, &size, totalSz)) <= 0) {
        XFREE(mac, pkcs12->heap, DYNAMIC_TYPE_PKCS);
        return ret;
    }
    mac->digestSz = size;
    mac->digest = (byte*)XMALLOC(mac->digestSz, pkcs12->heap,
                                 DYNAMIC_TYPE_DIGEST);
    if (mac->digest == NULL || mac->digestSz + curIdx > totalSz) {
        ERROR_OUT(MEMORY_E, exit_gsd);
    }
    XMEMCPY(mac->digest, mem + curIdx, mac->digestSz);

#ifdef WOLFSSL_DEBUG_PKCS12
    {
        byte* p;
        for (printf("\t\tDigest = "), p = (byte*)mem+curIdx;
             p < (byte*)mem + curIdx + mac->digestSz;
             printf("%02X", *p), p++);
        printf(" : size = %d\n", mac->digestSz);
    }
#endif

    curIdx += mac->digestSz;

    /* get salt, should be octet string */
    if (mem[curIdx++] != ASN_OCTET_STRING) {
        WOLFSSL_MSG("Failed to get salt");
        ERROR_OUT(ASN_PARSE_E, exit_gsd);
    }

    if ((ret = GetLength(mem, &curIdx, &size, totalSz)) <= 0) {
        goto exit_gsd;
    }
    mac->saltSz = size;
    mac->salt = (byte*)XMALLOC(mac->saltSz, pkcs12->heap, DYNAMIC_TYPE_SALT);
    if (mac->salt == NULL || mac->saltSz + curIdx > totalSz) {
        ERROR_OUT(MEMORY_E, exit_gsd);
    }
    XMEMCPY(mac->salt, mem + curIdx, mac->saltSz);

#ifdef WOLFSSL_DEBUG_PKCS12
    {
        byte* p;
        for (printf("\t\tSalt = "), p = (byte*)mem + curIdx;
             p < (byte*)mem + curIdx + mac->saltSz;
             printf("%02X", *p), p++);
        printf(" : size = %d\n", mac->saltSz);
    }
#endif

    curIdx += mac->saltSz;

    /* check for MAC itterations, default to 1 */
    mac->itt = 1;
    if (curIdx < totalSz) {
        int number = 0;
        if ((ret = GetShortInt(mem, &curIdx, &number, totalSz)) >= 0) {
            /* found a itteration value */
            mac->itt = number;
        }
    }

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("\t\tITTERATIONS : %d\n", mac->itt);
#endif

    *idx = curIdx;
    pkcs12->signData = mac;
    ret = 0; /* success */

exit_gsd:

    /* failure cleanup */
    if (ret != 0) {
        if (mac) {
            if (mac->digest)
                XFREE(mac->digest, pkcs12->heap, DYNAMIC_TYPE_DIGEST);
            XFREE(mac, pkcs12->heap, DYNAMIC_TYPE_PKCS);
        }
    }

    return ret;
}


/* check mac on pkcs12, pkcs12->mac has been sanity checked before entering *
 * returns the result of comparison, success is 0 */
static int wc_PKCS12_verify(WC_PKCS12* pkcs12, byte* data, word32 dataSz,
                            const byte* psw, word32 pswSz)
{
    Hmac     hmac;
    MacData* mac;
    int ret, typeH, kLen;
    int idx = 0;
    int id  = 3; /* value from RFC 7292 indicating key is used for MAC */
    word32 i;
    byte digest[MAX_DIGEST_SIZE];
    byte unicodePasswd[MAX_UNICODE_SZ];
    byte key[MAX_KEY_SIZE];

    mac = pkcs12->signData;

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("Verifying MAC with OID = %d\n", mac->oid);
#endif

    /* check if this builds digest size is too small */
    if (mac->digestSz > MAX_DIGEST_SIZE) {
        WOLFSSL_MSG("PKCS12 max digest size too small");
        return BAD_FUNC_ARG;
    }

    /* unicode set up from asn.c */
    if ((pswSz * 2 + 2) > (int)sizeof(unicodePasswd)) {
        WOLFSSL_MSG("PKCS12 max unicode size too small");
        return UNICODE_SIZE_E;
    }

    for (i = 0; i < pswSz; i++) {
        unicodePasswd[idx++] = 0x00;
        unicodePasswd[idx++] = (byte)psw[i];
    }
    /* add trailing NULL */
    unicodePasswd[idx++] = 0x00;
    unicodePasswd[idx++] = 0x00;

    /* get hash type used and resulting size of HMAC key */
    switch (mac->oid) {
    #ifndef NO_SHA
        case SHAh: /* 88 */
            typeH = SHA;
            kLen  = SHA_DIGEST_SIZE;
            break;
    #endif
    #ifndef NO_SHA256
        case SHA256h: /* 414 */
            typeH = SHA256;
            kLen  = SHA256_DIGEST_SIZE;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case SHA384h:  /* 415 */
            typeH = SHA384;
            kLen  = SHA384_DIGEST_SIZE;
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case SHA512h: /* 416 */
            typeH = SHA512;
            kLen  = SHA512_DIGEST_SIZE;
            break;
    #endif
        default: /* May be SHA224 or was just not built in */
            WOLFSSL_MSG("Unsupported hash used");
            return BAD_FUNC_ARG;
    }

    /* idx contains size of unicodePasswd */
    if ((ret = wc_PKCS12_PBKDF_ex(key, unicodePasswd, idx, mac->salt,
                   mac->saltSz, mac->itt, kLen, typeH, id, pkcs12->heap)) < 0) {
        return ret;
    }

    /* now that key has been created use it to get HMAC hash on data */
    if ((ret = wc_HmacInit(&hmac, pkcs12->heap, INVALID_DEVID)) != 0) {
        return ret;
    }
    ret = wc_HmacSetKey(&hmac, typeH, key, kLen);
    if (ret == 0)
        ret = wc_HmacUpdate(&hmac, data, dataSz);
    if (ret == 0)
        ret = wc_HmacFinal(&hmac, digest);
    wc_HmacFree(&hmac);

    if (ret != 0)
        return ret;

#ifdef WOLFSSL_DEBUG_PKCS12
    {
        byte* p;
        for (printf("\t\tHash = "), p = (byte*)digest;
             p < (byte*)digest + mac->digestSz;
             printf("%02X", *p), p++);
        printf(" : size = %d\n", mac->digestSz);
    }
#endif

    return XMEMCMP(digest, mac->digest, mac->digestSz);
}


/* Convert DER format stored in der buffer to WC_PKCS12 struct
 * Puts the raw contents of Content Info into structure without completly
 * parsing or decoding.
 * der    : pointer to der buffer holding PKCS12
 * derSz  : size of der buffer
 * pkcs12 : non-null pkcs12 pointer
 * return 0 on success and negative on failure.
 */
int wc_d2i_PKCS12(const byte* der, word32 derSz, WC_PKCS12* pkcs12)
{
    word32 idx  = 0;
    word32 totalSz = 0;
    int ret;
    int size    = 0;
    int version = 0;

    WOLFSSL_ENTER("wolfSSL_d2i_PKCS12_bio");

    if (der == NULL || pkcs12 == NULL) {
        return BAD_FUNC_ARG;
    }

    totalSz = derSz;
    if ((ret = GetSequence(der, &idx, &size, totalSz)) <= 0) {
        WOLFSSL_MSG("Failed to get PKCS12 sequence");
        return ret;
    }

    /* get version */
    if ((ret = GetMyVersion(der, &idx, &version, totalSz)) < 0) {
        return ret;
    }

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("\nBEGIN: PKCS12 size = %d\n", totalSz);
    printf("version = %d\n", version);
#endif

    if (version != 3) {
        WOLFSSL_MSG("PKCS12 unsupported version!");
        return ASN_VERSION_E;
    }

    if ((ret = GetSequence(der, &idx, &size, totalSz)) < 0) {
        return ret;
    }

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("\tSEQUENCE: AuthenticatedSafe size = %d\n", size);
#endif

    if ((ret = GetSafeContent(pkcs12, der, &idx, size + idx)) < 0) {
        WOLFSSL_MSG("GetSafeContent error");
        return ret;
    }

    /* if more buffer left check for MAC data */
    if (idx < totalSz) {
        if ((ret = GetSequence(der, &idx, &size, totalSz)) < 0) {
            WOLFSSL_MSG("Ignoring unknown data at end of PKCS12 DER buffer");
        }
        else {
        #ifdef WOLFSSL_DEBUG_PKCS12
            printf("\tSEQUENCE: Signature size = %d\n", size);
        #endif

            if ((ret = GetSignData(pkcs12, der, &idx, totalSz)) < 0) {
                return ASN_PARSE_E;
            }
        }
    }

#ifdef WOLFSSL_DEBUG_PKCS12
    printf("END: PKCS12\n");
#endif

    return ret;
}


/* helper function to free WC_DerCertList */
static void freeCertList(WC_DerCertList* list, void* heap)
{
    WC_DerCertList* current = list;
    WC_DerCertList* next;

    if (list == NULL) {
        return;
    }

    while (current != NULL) {
        next = current->next;
        if (current->buffer != NULL) {
            XFREE(current->buffer, heap, DYNAMIC_TYPE_PKCS);
        }
        XFREE(current, heap, DYNAMIC_TYPE_PKCS);
        current = next;
    }

    (void)heap;
}

static void freeDecCertList(WC_DerCertList** list, byte** pkey, word32* pkeySz,
    byte** cert, word32* certSz, void* heap)
{
    WC_DerCertList* current  = *list;
    WC_DerCertList* previous = NULL;
    DecodedCert DeCert;

    while (current != NULL) {

        InitDecodedCert(&DeCert, current->buffer, current->bufferSz, heap);
        if (ParseCertRelative(&DeCert, CERT_TYPE, NO_VERIFY, NULL) == 0) {
            if (wc_CheckPrivateKey(*pkey, *pkeySz, &DeCert) == 1) {
                WOLFSSL_MSG("Key Pair found");
                *cert = current->buffer;
                *certSz = current->bufferSz;

                if (previous == NULL) {
                    *list = current->next;
                }
                else {
                    previous->next = current->next;
                }
                FreeDecodedCert(&DeCert);
                XFREE(current, heap, DYNAMIC_TYPE_PKCS);
                break;
            }
        }
        FreeDecodedCert(&DeCert);

        previous = current;
        current  = current->next;
    }
}


/* return 0 on success and negative on failure.
 * By side effect returns private key, cert, and optionally ca.
 * Parses and decodes the parts of PKCS12
 *
 * NOTE: can parse with USER RSA enabled but may return cert that is not the
 *       pair for the key when using RSA key pairs.
 *
 * pkcs12 : non-null WC_PKCS12 struct
 * psw    : password to use for PKCS12 decode
 * pkey   : Private key returned
 * cert   : x509 cert returned
 * ca     : optional ca returned
 */
int wc_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
        byte** pkey, word32* pkeySz, byte** cert, word32* certSz,
        WC_DerCertList** ca)
{
    ContentInfo* ci       = NULL;
    WC_DerCertList* certList = NULL;
    byte* buf             = NULL;
    word32 i, oid;
    int ret, pswSz;

    WOLFSSL_ENTER("wc_PKCS12_parse");

    if (pkcs12 == NULL || psw == NULL || cert == NULL || certSz == NULL ||
        pkey == NULL   || pkeySz == NULL) {
        return BAD_FUNC_ARG;
    }

    pswSz = (int)XSTRLEN(psw);
    *cert = NULL;
    *pkey = NULL;
    if (ca != NULL)
        *ca = NULL;

    /* if there is sign data then verify the MAC */
    if (pkcs12->signData != NULL ) {
        if ((ret = wc_PKCS12_verify(pkcs12, pkcs12->safe->data,
                               pkcs12->safe->dataSz, (byte*)psw, pswSz)) != 0) {
            WOLFSSL_MSG("PKCS12 Bad MAC on verify");
            WOLFSSL_LEAVE("wc_PKCS12_parse verify ", ret);
            return MAC_CMP_FAILED_E;
        }
    }

    if (pkcs12->safe == NULL) {
        WOLFSSL_MSG("No PKCS12 safes to parse");
        return BAD_FUNC_ARG;
    }

    /* Decode content infos */
    ci = pkcs12->safe->CI;
    for (i = 0; i < pkcs12->safe->numCI; i++) {
        byte*  data;
        word32 idx = 0;
        int    size, totalSz;

        if (ci->type == WC_PKCS12_ENCRYPTED_DATA) {
            int number;

            WOLFSSL_MSG("Decrypting PKCS12 Content Info Container");
            data = ci->data;
            if (data[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                ERROR_OUT(ASN_PARSE_E, exit_pk12par);
            }
            if ((ret = GetLength(data, &idx, &size, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

            if ((ret = GetSequence(data, &idx, &size, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

            if ((ret = GetShortInt(data, &idx, &number, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

            if (number != 0) {
                WOLFSSL_MSG("Expecting 0 for Integer with Encrypted PKCS12");
            }

            if ((ret = GetSequence(data, &idx, &size, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

            ret = GetObjectId(data, &idx, &oid, oidIgnoreType, ci->dataSz);
            if (ret < 0 || oid != WC_PKCS12_DATA) {
                WOLFSSL_MSG("Not PKCS12 DATA object or get object parse error");
                ERROR_OUT(ASN_PARSE_E, exit_pk12par);
            }

            /* decrypted content overwrites input buffer */
            size = ci->dataSz - idx;
            buf = (byte*)XMALLOC(size, pkcs12->heap, DYNAMIC_TYPE_PKCS);
            if (buf == NULL) {
                ERROR_OUT(MEMORY_E, exit_pk12par);
            }
            XMEMCPY(buf, data + idx, size);

            if ((ret = DecryptContent(buf, size, psw, pswSz)) < 0) {
                WOLFSSL_MSG("Decryption failed, algorithm not compiled in?");
                goto exit_pk12par;
            }

            data = buf;
            idx = 0;

        #ifdef WOLFSSL_DEBUG_PKCS12
            {
                byte* p;
                for (printf("\tData = "), p = (byte*)buf;
                    p < (byte*)buf + size;
                    printf("%02X", *p), p++);
                printf("\n");
            }
        #endif
        }
        else { /* type DATA */
            WOLFSSL_MSG("Parsing PKCS12 DATA Content Info Container");
            data = ci->data;
            if (data[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                ERROR_OUT(ASN_PARSE_E, exit_pk12par);
            }
            if ((ret = GetLength(data, &idx, &size, ci->dataSz)) <= 0) {
                goto exit_pk12par;
            }
            if (data[idx++] != ASN_OCTET_STRING) {
                ERROR_OUT(ASN_PARSE_E, exit_pk12par);
            }
            if ((ret = GetLength(data, &idx, &size, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

        }

        /* parse through bags in ContentInfo */
        if ((ret = GetSequence(data, &idx, &totalSz, ci->dataSz)) < 0) {
            goto exit_pk12par;
        }
        totalSz += idx;

        while ((int)idx < totalSz) {
            int bagSz;
            if ((ret = GetSequence(data, &idx, &bagSz, ci->dataSz)) < 0) {
                goto exit_pk12par;
            }
            bagSz += idx;

            if ((ret = GetObjectId(data, &idx, &oid, oidIgnoreType,
                                                             ci->dataSz)) < 0) {
                goto exit_pk12par;
            }

            switch (oid) {
                case WC_PKCS12_KeyBag: /* 667 */
                    WOLFSSL_MSG("PKCS12 Key Bag found");
                    if (data[idx++] !=
                                     (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                        ERROR_OUT(ASN_PARSE_E, exit_pk12par);
                    }
                    if ((ret = GetLength(data, &idx, &size, ci->dataSz)) <= 0) {
                        goto exit_pk12par;
                    }
                    if (*pkey == NULL) {
                        *pkey = (byte*)XMALLOC(size, pkcs12->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
                        if (*pkey == NULL) {
                            ERROR_OUT(MEMORY_E, exit_pk12par);
                        }
                        XMEMCPY(*pkey, data + idx, size);
                        *pkeySz =  ToTraditional(*pkey, size);
                    }

                #ifdef WOLFSSL_DEBUG_PKCS12
                    {
                        byte* p;
                        for (printf("\tKey = "), p = (byte*)*pkey;
                            p < (byte*)*pkey + size;
                            printf("%02X", *p), p++);
                        printf("\n");
                    }
                #endif
                    idx += size;
                    break;

                case WC_PKCS12_ShroudedKeyBag: /* 668 */
                    {
                        byte* k;

                        WOLFSSL_MSG("PKCS12 Shrouded Key Bag found");
                        if (data[idx++] !=
                                     (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                            ERROR_OUT(ASN_PARSE_E, exit_pk12par);
                        }
                        if ((ret = GetLength(data, &idx, &size,
                                                             ci->dataSz)) < 0) {
                            goto exit_pk12par;
                        }

                        k = (byte*)XMALLOC(size, pkcs12->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
                        if (k == NULL) {
                            ERROR_OUT(MEMORY_E, exit_pk12par);
                        }
                        XMEMCPY(k, data + idx, size);

                        /* overwrites input, be warned */
                        if ((ret = ToTraditionalEnc(k, size, psw, pswSz)) < 0) {
                            XFREE(k, pkcs12->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                            goto exit_pk12par;
                        }

                        if (ret < size) {
                            /* shrink key buffer */
                            byte* tmp = (byte*)XMALLOC(ret, pkcs12->heap,
                                                 DYNAMIC_TYPE_PUBLIC_KEY);
                            if (tmp == NULL) {
                                XFREE(k, pkcs12->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                                ERROR_OUT(MEMORY_E, exit_pk12par);
                            }
                            XMEMCPY(tmp, k, ret);
                            XFREE(k, pkcs12->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                            k = tmp;
                        }
                        size = ret;

                        if (*pkey == NULL) {
                            *pkey = k;
                            *pkeySz = size;
                        }
                        else { /* only expecting one key */
                            XFREE(k, pkcs12->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                        }
                        idx += size;

                    #ifdef WOLFSSL_DEBUG_PKCS12
                        {
                            byte* p;
                            for (printf("\tKey = "), p = (byte*)k;
                                p < (byte*)k + ret;
                                printf("%02X", *p), p++);
                            printf("\n");
                        }
                    #endif
                    }
                    break;

                case WC_PKCS12_CertBag: /* 669 */
                {
                    WC_DerCertList* node;
                    WOLFSSL_MSG("PKCS12 Cert Bag found");
                    if (data[idx++] !=
                                     (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                        ERROR_OUT(ASN_PARSE_E, exit_pk12par);
                    }
                    if ((ret = GetLength(data, &idx, &size, ci->dataSz)) < 0) {
                        goto exit_pk12par;
                    }

                    /* get cert bag type */
                    if ((ret = GetSequence(data, &idx, &size, ci->dataSz)) <0) {
                        goto exit_pk12par;
                    }

                    if ((ret = GetObjectId(data, &idx, &oid, oidIgnoreType,
                                                             ci->dataSz)) < 0) {
                        goto exit_pk12par;
                    }

                    switch (oid) {
                        case WC_PKCS12_CertBag_Type1:  /* 675 */
                            /* type 1 */
                            WOLFSSL_MSG("PKCS12 cert bag type 1");
                            if (data[idx++] !=
                                     (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC)) {
                                ERROR_OUT(ASN_PARSE_E, exit_pk12par);
                            }
                            if ((ret = GetLength(data, &idx, &size, ci->dataSz))
                                                                         <= 0) {
                                goto exit_pk12par;
                            }
                            if (data[idx++] != ASN_OCTET_STRING) {
                                ERROR_OUT(ASN_PARSE_E, exit_pk12par);

                            }
                            if ((ret = GetLength(data, &idx, &size, ci->dataSz))
                                                                          < 0) {
                                goto exit_pk12par;
                            }
                            break;
                       default:
                            WOLFSSL_MSG("Unknown PKCS12 cert bag type");
                    }

                    if (size + idx > (word32)bagSz) {
                        ERROR_OUT(ASN_PARSE_E, exit_pk12par);
                    }

                    /* list to hold all certs found */
                    node = (WC_DerCertList*)XMALLOC(sizeof(WC_DerCertList),
                                               pkcs12->heap, DYNAMIC_TYPE_PKCS);
                    if (node == NULL) {
                        ERROR_OUT(MEMORY_E, exit_pk12par);
                    }
                    XMEMSET(node, 0, sizeof(WC_DerCertList));

                    node->buffer = (byte*)XMALLOC(size, pkcs12->heap,
                                                             DYNAMIC_TYPE_PKCS);
                    if (node->buffer == NULL) {
                        XFREE(node, pkcs12->heap, DYNAMIC_TYPE_PKCS);
                        ERROR_OUT(MEMORY_E, exit_pk12par);
                    }
                    XMEMCPY(node->buffer, data + idx, size);
                    node->bufferSz = size;

                    /* put the new node into the list */
                    if (certList != NULL) {
                        WOLFSSL_MSG("Pushing new cert onto stack");
                        node->next = certList;
                        certList = node;
                    }
                    else {
                        certList = node;
                    }

                    /* on to next */
                    idx += size;
                }
                    break;

                case WC_PKCS12_CrlBag: /* 670 */
                    WOLFSSL_MSG("PKCS12 CRL BAG not yet supported");
                    break;

                case WC_PKCS12_SecretBag: /* 671 */
                    WOLFSSL_MSG("PKCS12 Secret BAG not yet supported");
                    break;

                case WC_PKCS12_SafeContentsBag: /* 672 */
                    WOLFSSL_MSG("PKCS12 Safe Contents BAG not yet supported");
                    break;

                default:
                    WOLFSSL_MSG("Unknown PKCS12 BAG type found");
            }

            /* Attribute, unknown bag or unsupported */
            if ((int)idx < bagSz) {
                idx = bagSz; /* skip for now */
            }
        }

        /* free temporary buffer */
        if (buf != NULL) {
            XFREE(buf, pkcs12->heap, DYNAMIC_TYPE_PKCS);
            buf = NULL;
        }

        ci = ci->next;
        WOLFSSL_MSG("Done Parsing PKCS12 Content Info Container");
    }

    /* check if key pair, remove from list */
    if (*pkey != NULL) {
        freeDecCertList(&certList, pkey, pkeySz, cert, certSz, pkcs12->heap);
    }

    /* if ca arg provided return certList, otherwise free it */
    if (ca != NULL) {
        *ca = certList;
    }
    else {
        /* free list, not wanted */
        freeCertList(certList, pkcs12->heap);
    }

    ret = 0; /* success */

exit_pk12par:

    if (ret != 0) {
        /* failure cleanup */
        if (*pkey) {
            XFREE(*pkey, pkcs12->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            *pkey = NULL;
        }
        if (buf) {
            XFREE(buf, pkcs12->heap, DYNAMIC_TYPE_PKCS);
            buf = NULL;
        }

        freeCertList(certList, pkcs12->heap);
    }

    return ret;
}


/* if using a specific memory heap */
int wc_PKCS12_SetHeap(WC_PKCS12* pkcs12, void* heap)
{
    if (pkcs12 == NULL) {
        return BAD_FUNC_ARG;
    }
    pkcs12->heap = heap;

    return 0;
}


/* getter for heap */
void* wc_PKCS12_GetHeap(WC_PKCS12* pkcs12)
{
    if (pkcs12 == NULL) {
        return NULL;
    }

    return pkcs12->heap;
}

#undef ERROR_OUT

#endif /* !NO_ASN && !NO_PWDBASED */
