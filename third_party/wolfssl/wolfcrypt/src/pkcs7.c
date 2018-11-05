/* pkcs7.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifdef HAVE_PKCS7

#include <wolfssl/wolfcrypt/pkcs7.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/hash.h>
#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


/* direction for processing, encoding or decoding */
typedef enum {
    WC_PKCS7_ENCODE,
    WC_PKCS7_DECODE
} pkcs7Direction;

#define MAX_PKCS7_DIGEST_SZ (MAX_SEQ_SZ + MAX_ALGO_SZ + \
                             MAX_OCTET_STR_SZ + WC_MAX_DIGEST_SIZE)


/* placed ASN.1 contentType OID into *output, return idx on success,
 * 0 upon failure */
static int wc_SetContentType(int pkcs7TypeOID, byte* output)
{
    /* PKCS#7 content types, RFC 2315, section 14 */
    const byte pkcs7[]              = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07 };
    const byte data[]               = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x01 };
    const byte signedData[]         = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x02};
    const byte envelopedData[]      = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x03 };
    const byte signedAndEnveloped[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x04 };
    const byte digestedData[]       = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x05 };

#ifndef NO_PKCS7_ENCRYPTED_DATA
    const byte encryptedData[]      = { 0x2A, 0x86, 0x48, 0x86, 0xF7,
                                               0x0D, 0x01, 0x07, 0x06 };
#endif

    int idSz;
    int typeSz = 0, idx = 0;
    const byte* typeName = 0;
    byte ID_Length[MAX_LENGTH_SZ];

    switch (pkcs7TypeOID) {
        case PKCS7_MSG:
            typeSz = sizeof(pkcs7);
            typeName = pkcs7;
            break;

        case DATA:
            typeSz = sizeof(data);
            typeName = data;
            break;

        case SIGNED_DATA:
            typeSz = sizeof(signedData);
            typeName = signedData;
            break;

        case ENVELOPED_DATA:
            typeSz = sizeof(envelopedData);
            typeName = envelopedData;
            break;

        case SIGNED_AND_ENVELOPED_DATA:
            typeSz = sizeof(signedAndEnveloped);
            typeName = signedAndEnveloped;
            break;

        case DIGESTED_DATA:
            typeSz = sizeof(digestedData);
            typeName = digestedData;
            break;

#ifndef NO_PKCS7_ENCRYPTED_DATA
        case ENCRYPTED_DATA:
            typeSz = sizeof(encryptedData);
            typeName = encryptedData;
            break;
#endif

        default:
            WOLFSSL_MSG("Unknown PKCS#7 Type");
            return 0;
    };

    idSz  = SetLength(typeSz, ID_Length);
    output[idx++] = ASN_OBJECT_ID;
    XMEMCPY(output + idx, ID_Length, idSz);
    idx += idSz;
    XMEMCPY(output + idx, typeName, typeSz);
    idx += typeSz;

    return idx;
}


/* get ASN.1 contentType OID sum, return 0 on success, <0 on failure */
static int wc_GetContentType(const byte* input, word32* inOutIdx, word32* oid,
                             word32 maxIdx)
{
    WOLFSSL_ENTER("wc_GetContentType");
    if (GetObjectId(input, inOutIdx, oid, oidIgnoreType, maxIdx) < 0)
        return ASN_PARSE_E;

    return 0;
}


/* return block size for algorithm represented by oid, or <0 on error */
static int wc_PKCS7_GetOIDBlockSize(int oid)
{
    int blockSz;

    switch (oid) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128CBCb:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192CBCb:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256CBCb:
    #endif
            blockSz = AES_BLOCK_SIZE;
            break;
#endif
#ifndef NO_DES3
        case DESb:
        case DES3b:
            blockSz = DES_BLOCK_SIZE;
            break;
#endif
        default:
            WOLFSSL_MSG("Unsupported content cipher type");
            return ALGO_ID_E;
    };

    return blockSz;
}


/* get key size for algorithm represented by oid, or <0 on error */
static int wc_PKCS7_GetOIDKeySize(int oid)
{
    int blockKeySz;

    switch (oid) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128CBCb:
        case AES128_WRAP:
            blockKeySz = 16;
            break;
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192CBCb:
        case AES192_WRAP:
            blockKeySz = 24;
            break;
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256CBCb:
        case AES256_WRAP:
            blockKeySz = 32;
            break;
    #endif
#endif
#ifndef NO_DES3
        case DESb:
            blockKeySz = DES_KEYLEN;
            break;

        case DES3b:
            blockKeySz = DES3_KEYLEN;
            break;
#endif
        default:
            WOLFSSL_MSG("Unsupported content cipher type");
            return ALGO_ID_E;
    };

    return blockKeySz;
}


/* This is to initialize a PKCS7 structure. It sets all values to 0 and can be
 * used to set the heap hint.
 *
 * pkcs7 PKCS7 structure to initialize
 * heap  memory heap hint for PKCS7 structure to use
 * devId currently not used but a place holder for async operations
 *
 * returns 0 on success or a negative value for failure
 */
int wc_PKCS7_Init(PKCS7* pkcs7, void* heap, int devId)
{
    WOLFSSL_ENTER("wc_PKCS7_Init");

    if (pkcs7 == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(pkcs7, 0, sizeof(PKCS7));
    pkcs7->heap = heap;
    pkcs7->devId = devId;

    return 0;
}


/* init PKCS7 struct with recipient cert, decode into DecodedCert
 * NOTE: keeps previously set pkcs7 memory heap hint */
int wc_PKCS7_InitWithCert(PKCS7* pkcs7, byte* cert, word32 certSz)
{
    int ret = 0;
    void* heap;
    int devId;

    if (pkcs7 == NULL || (cert == NULL && certSz != 0)) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_HEAP_TEST
    heap = (void*)WOLFSSL_HEAP_TEST;
#else
    heap = pkcs7->heap;
#endif
    devId = pkcs7->devId;

    XMEMSET(pkcs7, 0, sizeof(PKCS7));
    pkcs7->heap = heap;
    pkcs7->devId = devId;

    if (cert != NULL && certSz > 0) {
#ifdef WOLFSSL_SMALL_STACK
        DecodedCert* dCert;

        dCert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_PKCS7);
        if (dCert == NULL)
            return MEMORY_E;
#else
        DecodedCert stack_dCert;
        DecodedCert* dCert = &stack_dCert;
#endif

        pkcs7->singleCert = cert;
        pkcs7->singleCertSz = certSz;
        InitDecodedCert(dCert, cert, certSz, pkcs7->heap);

        ret = ParseCert(dCert, CA_TYPE, NO_VERIFY, 0);
        if (ret < 0) {
            FreeDecodedCert(dCert);
#ifdef WOLFSSL_SMALL_STACK
            XFREE(dCert, NULL, DYNAMIC_TYPE_PKCS7);
#endif
            return ret;
        }

        XMEMCPY(pkcs7->publicKey, dCert->publicKey, dCert->pubKeySize);
        pkcs7->publicKeySz = dCert->pubKeySize;
        pkcs7->publicKeyOID = dCert->keyOID;
        XMEMCPY(pkcs7->issuerHash, dCert->issuerHash, KEYID_SIZE);
        pkcs7->issuer = dCert->issuerRaw;
        pkcs7->issuerSz = dCert->issuerRawLen;
        XMEMCPY(pkcs7->issuerSn, dCert->serial, dCert->serialSz);
        pkcs7->issuerSnSz = dCert->serialSz;
        FreeDecodedCert(dCert);

#ifdef WOLFSSL_SMALL_STACK
        XFREE(dCert, NULL, DYNAMIC_TYPE_PKCS7);
#endif
    }

    return ret;
}


/* free linked list of PKCS7DecodedAttrib structs */
static void wc_PKCS7_FreeDecodedAttrib(PKCS7DecodedAttrib* attrib, void* heap)
{
    PKCS7DecodedAttrib* current;

    if (attrib == NULL) {
        return;
    }

    current = attrib;
    while (current != NULL) {
        PKCS7DecodedAttrib* next = current->next;
        if (current->oid != NULL)  {
            XFREE(current->oid, heap, DYNAMIC_TYPE_PKCS7);
        }
        if (current->value != NULL) {
            XFREE(current->value, heap, DYNAMIC_TYPE_PKCS7);
        }
        XFREE(current, heap, DYNAMIC_TYPE_PKCS7);
        current = next;
    }

    (void)heap;
}


/* releases any memory allocated by a PKCS7 initializer */
void wc_PKCS7_Free(PKCS7* pkcs7)
{
    if (pkcs7 == NULL)
        return;

    wc_PKCS7_FreeDecodedAttrib(pkcs7->decodedAttrib, pkcs7->heap);

#ifdef ASN_BER_TO_DER
    if (pkcs7->der != NULL)
        XFREE(pkcs7->der, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#endif
}


/* helper function for parsing through attributes and finding a specific one.
 * returns PKCS7DecodedAttrib pointer on success */
static PKCS7DecodedAttrib* findAttrib(PKCS7* pkcs7, const byte* oid, word32 oidSz)
{
    PKCS7DecodedAttrib* list;

    if (pkcs7 == NULL || oid == NULL) {
        return NULL;
    }

    /* search attributes for pkiStatus */
    list = pkcs7->decodedAttrib;
    while (list != NULL) {
        word32 sz  = oidSz;
        word32 idx = 0;
        int    length = 0;

        if (list->oid[idx++] != ASN_OBJECT_ID) {
            WOLFSSL_MSG("Bad attribute ASN1 syntax");
            return NULL;
        }

        if (GetLength(list->oid, &idx, &length, list->oidSz) < 0) {
            WOLFSSL_MSG("Bad attribute length");
            return NULL;
        }

        sz = (sz < (word32)length)? sz : (word32)length;
        if (XMEMCMP(oid, list->oid + idx, sz) == 0) {
            return list;
        }
        list = list->next;
    }
    return NULL;
}


/* Searches through decoded attributes and returns the value for the first one
 * matching the oid passed in. Note that this value includes the leading ASN1
 * syntax. So for a printable string of "3" this would be something like
 *
 * 0x13, 0x01, 0x33
 *  ID   SIZE  "3"
 *
 * pkcs7  structure to get value from
 * oid    OID value to search for with attributes
 * oidSz  size of oid buffer
 * out    buffer to hold result
 * outSz  size of out buffer (if out is NULL this is set to needed size and
          LENGTH_ONLY_E is returned)
 *
 * returns size of value on success
 */
int wc_PKCS7_GetAttributeValue(PKCS7* pkcs7, const byte* oid, word32 oidSz,
        byte* out, word32* outSz)
{
    PKCS7DecodedAttrib* attrib;

    if (pkcs7 == NULL || oid == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    attrib = findAttrib(pkcs7, oid, oidSz);
    if (attrib == NULL) {
        return ASN_PARSE_E;
    }

    if (out == NULL) {
        *outSz = attrib->valueSz;
        return LENGTH_ONLY_E;
    }

    if (*outSz < attrib->valueSz) {
        return BUFFER_E;
    }

    XMEMCPY(out, attrib->value, attrib->valueSz);
    return attrib->valueSz;
}


/* build PKCS#7 data content type */
int wc_PKCS7_EncodeData(PKCS7* pkcs7, byte* output, word32 outputSz)
{
    static const byte oid[] =
        { ASN_OBJECT_ID, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01,
                         0x07, 0x01 };
    byte seq[MAX_SEQ_SZ];
    byte octetStr[MAX_OCTET_STR_SZ];
    word32 seqSz;
    word32 octetStrSz;
    word32 oidSz = (word32)sizeof(oid);
    int idx = 0;

    if (pkcs7 == NULL || output == NULL) {
        return BAD_FUNC_ARG;
    }

    octetStrSz = SetOctetString(pkcs7->contentSz, octetStr);
    seqSz = SetSequence(pkcs7->contentSz + octetStrSz + oidSz, seq);

    if (outputSz < pkcs7->contentSz + octetStrSz + oidSz + seqSz)
        return BUFFER_E;

    XMEMCPY(output, seq, seqSz);
    idx += seqSz;
    XMEMCPY(output + idx, oid, oidSz);
    idx += oidSz;
    XMEMCPY(output + idx, octetStr, octetStrSz);
    idx += octetStrSz;
    XMEMCPY(output + idx, pkcs7->content, pkcs7->contentSz);
    idx += pkcs7->contentSz;

    return idx;
}


typedef struct EncodedAttrib {
    byte valueSeq[MAX_SEQ_SZ];
        const byte* oid;
        byte valueSet[MAX_SET_SZ];
        const byte* value;
    word32 valueSeqSz, oidSz, idSz, valueSetSz, valueSz, totalSz;
} EncodedAttrib;


typedef struct ESD {
    wc_HashAlg  hash;
    enum wc_HashType hashType;
    byte contentDigest[WC_MAX_DIGEST_SIZE + 2]; /* content only + ASN.1 heading */
    byte contentAttribsDigest[WC_MAX_DIGEST_SIZE];
    byte encContentDigest[MAX_ENCRYPTED_KEY_SZ];

    byte outerSeq[MAX_SEQ_SZ];
        byte outerContent[MAX_EXP_SZ];
            byte innerSeq[MAX_SEQ_SZ];
                byte version[MAX_VERSION_SZ];
                byte digAlgoIdSet[MAX_SET_SZ];
                    byte singleDigAlgoId[MAX_ALGO_SZ];

                byte contentInfoSeq[MAX_SEQ_SZ];
                    byte innerContSeq[MAX_EXP_SZ];
                        byte innerOctets[MAX_OCTET_STR_SZ];

                byte certsSet[MAX_SET_SZ];

                byte signerInfoSet[MAX_SET_SZ];
                    byte signerInfoSeq[MAX_SEQ_SZ];
                        byte signerVersion[MAX_VERSION_SZ];
                        byte issuerSnSeq[MAX_SEQ_SZ];
                            byte issuerName[MAX_SEQ_SZ];
                            byte issuerSn[MAX_SN_SZ];
                        byte signerDigAlgoId[MAX_ALGO_SZ];
                        byte digEncAlgoId[MAX_ALGO_SZ];
                        byte signedAttribSet[MAX_SET_SZ];
                            EncodedAttrib signedAttribs[6];
                        byte signerDigest[MAX_OCTET_STR_SZ];
    word32 innerOctetsSz, innerContSeqSz, contentInfoSeqSz;
    word32 outerSeqSz, outerContentSz, innerSeqSz, versionSz, digAlgoIdSetSz,
           singleDigAlgoIdSz, certsSetSz;
    word32 signerInfoSetSz, signerInfoSeqSz, signerVersionSz,
           issuerSnSeqSz, issuerNameSz, issuerSnSz,
           signerDigAlgoIdSz, digEncAlgoIdSz, signerDigestSz;
    word32 encContentDigestSz, signedAttribsSz, signedAttribsCount,
           signedAttribSetSz;
} ESD;


static int EncodeAttributes(EncodedAttrib* ea, int eaSz,
                                            PKCS7Attrib* attribs, int attribsSz)
{
    int i;
    int maxSz = min(eaSz, attribsSz);
    int allAttribsSz = 0;

    for (i = 0; i < maxSz; i++)
    {
        int attribSz = 0;

        ea[i].value = attribs[i].value;
        ea[i].valueSz = attribs[i].valueSz;
        attribSz += ea[i].valueSz;
        ea[i].valueSetSz = SetSet(attribSz, ea[i].valueSet);
        attribSz += ea[i].valueSetSz;
        ea[i].oid = attribs[i].oid;
        ea[i].oidSz = attribs[i].oidSz;
        attribSz += ea[i].oidSz;
        ea[i].valueSeqSz = SetSequence(attribSz, ea[i].valueSeq);
        attribSz += ea[i].valueSeqSz;
        ea[i].totalSz = attribSz;

        allAttribsSz += attribSz;
    }
    return allAttribsSz;
}


static int FlattenAttributes(byte* output, EncodedAttrib* ea, int eaSz)
{
    int i, idx;

    idx = 0;
    for (i = 0; i < eaSz; i++) {
        XMEMCPY(output + idx, ea[i].valueSeq, ea[i].valueSeqSz);
        idx += ea[i].valueSeqSz;
        XMEMCPY(output + idx, ea[i].oid, ea[i].oidSz);
        idx += ea[i].oidSz;
        XMEMCPY(output + idx, ea[i].valueSet, ea[i].valueSetSz);
        idx += ea[i].valueSetSz;
        XMEMCPY(output + idx, ea[i].value, ea[i].valueSz);
        idx += ea[i].valueSz;
    }
    return 0;
}


#ifndef NO_RSA

/* returns size of signature put into out, negative on error */
static int wc_PKCS7_RsaSign(PKCS7* pkcs7, byte* in, word32 inSz, ESD* esd)
{
    int ret;
    word32 idx;
#ifdef WOLFSSL_SMALL_STACK
    RsaKey* privKey;
#else
    RsaKey  stack_privKey;
    RsaKey* privKey = &stack_privKey;
#endif

    if (pkcs7 == NULL || pkcs7->rng == NULL || in == NULL || esd == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    privKey = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (privKey == NULL)
        return MEMORY_E;
#endif

    ret = wc_InitRsaKey_ex(privKey, pkcs7->heap, pkcs7->devId);
    if (ret == 0) {
        if (pkcs7->privateKey != NULL && pkcs7->privateKeySz > 0) {
            idx = 0;
            ret = wc_RsaPrivateKeyDecode(pkcs7->privateKey, &idx, privKey,
                                         pkcs7->privateKeySz);
        }
        else if (pkcs7->devId == INVALID_DEVID) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        ret = wc_RsaSSL_Sign(in, inSz, esd->encContentDigest,
                             sizeof(esd->encContentDigest),
                             privKey, pkcs7->rng);
    }

    wc_FreeRsaKey(privKey);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* NO_RSA */


#ifdef HAVE_ECC

/* returns size of signature put into out, negative on error */
static int wc_PKCS7_EcdsaSign(PKCS7* pkcs7, byte* in, word32 inSz, ESD* esd)
{
    int ret;
    word32 outSz, idx;
#ifdef WOLFSSL_SMALL_STACK
    ecc_key* privKey;
#else
    ecc_key  stack_privKey;
    ecc_key* privKey = &stack_privKey;
#endif

    if (pkcs7 == NULL || pkcs7->rng == NULL || in == NULL || esd == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    privKey = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (privKey == NULL)
        return MEMORY_E;
#endif

    ret = wc_ecc_init_ex(privKey, pkcs7->heap, pkcs7->devId);
    if (ret == 0) {
        if (pkcs7->privateKey != NULL && pkcs7->privateKeySz > 0) {
            idx = 0;
            ret = wc_EccPrivateKeyDecode(pkcs7->privateKey, &idx, privKey,
                                         pkcs7->privateKeySz);
        }
        else if (pkcs7->devId == INVALID_DEVID) {
            ret = BAD_FUNC_ARG;
        }
    }
    if (ret == 0) {
        outSz = sizeof(esd->encContentDigest);
        ret = wc_ecc_sign_hash(in, inSz, esd->encContentDigest,
                               &outSz, pkcs7->rng, privKey);
        if (ret == 0)
            ret = (int)outSz;
    }

    wc_ecc_free(privKey);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* HAVE_ECC */


/* builds up SignedData signed attributes, including default ones.
 *
 * pkcs7 - pointer to initialized PKCS7 structure
 * esd   - pointer to initialized ESD structure, used for output
 *
 * return 0 on success, negative on error */
static int wc_PKCS7_BuildSignedAttributes(PKCS7* pkcs7, ESD* esd,
                    byte* contentTypeOid, word32 contentTypeOidSz,
                    byte* contentType, word32 contentTypeSz,
                    byte* messageDigestOid, word32 messageDigestOidSz)
{
    int hashSz;

    PKCS7Attrib cannedAttribs[2];
    word32 cannedAttribsCount;

    if (pkcs7 == NULL || esd == NULL || contentTypeOid == NULL ||
        contentType == NULL || messageDigestOid == NULL)
        return BAD_FUNC_ARG;

    hashSz = wc_HashGetDigestSize(esd->hashType);
    if (hashSz < 0)
        return hashSz;

    cannedAttribsCount = sizeof(cannedAttribs)/sizeof(PKCS7Attrib);

    cannedAttribs[0].oid     = contentTypeOid;
    cannedAttribs[0].oidSz   = contentTypeOidSz;
    cannedAttribs[0].value   = contentType;
    cannedAttribs[0].valueSz = contentTypeSz;
    cannedAttribs[1].oid     = messageDigestOid;
    cannedAttribs[1].oidSz   = messageDigestOidSz;
    cannedAttribs[1].value   = esd->contentDigest;
    cannedAttribs[1].valueSz = hashSz + 2;  /* ASN.1 heading */

    esd->signedAttribsCount += cannedAttribsCount;
    esd->signedAttribsSz += EncodeAttributes(&esd->signedAttribs[0], 2,
                                         cannedAttribs, cannedAttribsCount);

    esd->signedAttribsCount += pkcs7->signedAttribsSz;
    esd->signedAttribsSz += EncodeAttributes(&esd->signedAttribs[2], 4,
                              pkcs7->signedAttribs, pkcs7->signedAttribsSz);

    return 0;
}


/* gets correct encryption algo ID for SignedData, either CTC_<hash>wRSA or
 * CTC_<hash>wECDSA, from pkcs7->publicKeyOID and pkcs7->hashOID.
 *
 * pkcs7          - pointer to PKCS7 structure
 * digEncAlgoId   - [OUT] output int to store correct algo ID in
 * digEncAlgoType - [OUT] output for algo ID type
 *
 * return 0 on success, negative on error */
static int wc_PKCS7_SignedDataGetEncAlgoId(PKCS7* pkcs7, int* digEncAlgoId,
                                           int* digEncAlgoType)
{
    int algoId   = 0;
    int algoType = 0;

    if (pkcs7 == NULL || digEncAlgoId == NULL || digEncAlgoType == NULL)
        return BAD_FUNC_ARG;

    if (pkcs7->publicKeyOID == RSAk) {

        algoType = oidSigType;

        switch (pkcs7->hashOID) {
        #ifndef NO_SHA
            case SHAh:
                algoId = CTC_SHAwRSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA224
            case SHA224h:
                algoId = CTC_SHA224wRSA;
                break;
        #endif
        #ifndef NO_SHA256
            case SHA256h:
                algoId = CTC_SHA256wRSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA384
            case SHA384h:
                algoId = CTC_SHA384wRSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA512
            case SHA512h:
                algoId = CTC_SHA512wRSA;
                break;
        #endif
        }

    }
#ifdef HAVE_ECC
    else if (pkcs7->publicKeyOID == ECDSAk) {

        algoType = oidSigType;

        switch (pkcs7->hashOID) {
        #ifndef NO_SHA
            case SHAh:
                algoId = CTC_SHAwECDSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA224
            case SHA224h:
                algoId = CTC_SHA224wECDSA;
                break;
        #endif
        #ifndef NO_SHA256
            case SHA256h:
                algoId = CTC_SHA256wECDSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA384
            case SHA384h:
                algoId = CTC_SHA384wECDSA;
                break;
        #endif
        #ifdef WOLFSSL_SHA512
            case SHA512h:
                algoId = CTC_SHA512wECDSA;
                break;
        #endif
        }
    }
#endif /* HAVE_ECC */

    if (algoId == 0) {
        WOLFSSL_MSG("Invalid signature algorithm type");
        return BAD_FUNC_ARG;
    }

    *digEncAlgoId = algoId;
    *digEncAlgoType = algoType;

    return 0;
}


/* build SignedData DigestInfo for use with PKCS#7/RSA
 *
 * pkcs7 - pointer to initialized PKCS7 struct
 * flatSignedAttribs - flattened, signed attributes
 * flatSignedAttrbsSz - size of flatSignedAttribs, octets
 * esd - pointer to initialized ESD struct
 * digestInfo - [OUT] output array for DigestInfo
 * digestInfoSz - [IN/OUT] - input size of array, size of digestInfo
 *
 * return 0 on success, negative on error */
static int wc_PKCS7_BuildDigestInfo(PKCS7* pkcs7, byte* flatSignedAttribs,
                                    word32 flatSignedAttribsSz, ESD* esd,
                                    byte* digestInfo, word32* digestInfoSz)
{
    int ret, hashSz, digIdx = 0;
    byte digestInfoSeq[MAX_SEQ_SZ];
    byte digestStr[MAX_OCTET_STR_SZ];
    byte attribSet[MAX_SET_SZ];
    byte algoId[MAX_ALGO_SZ];
    word32 digestInfoSeqSz, digestStrSz, algoIdSz;
    word32 attribSetSz;

    if (pkcs7 == NULL || esd == NULL || digestInfo == NULL ||
        digestInfoSz == NULL) {
        return BAD_FUNC_ARG;
    }

    hashSz = wc_HashGetDigestSize(esd->hashType);
    if (hashSz < 0)
        return hashSz;

    if (pkcs7->signedAttribsSz != 0) {

        if (flatSignedAttribs == NULL)
            return BAD_FUNC_ARG;

        attribSetSz = SetSet(flatSignedAttribsSz, attribSet);

        ret = wc_HashInit(&esd->hash, esd->hashType);
        if (ret < 0)
            return ret;

        ret = wc_HashUpdate(&esd->hash, esd->hashType,
                            attribSet, attribSetSz);
        if (ret < 0)
            return ret;

        ret = wc_HashUpdate(&esd->hash, esd->hashType,
                            flatSignedAttribs, flatSignedAttribsSz);
        if (ret < 0)
            return ret;

        ret = wc_HashFinal(&esd->hash, esd->hashType,
                           esd->contentAttribsDigest);
        if (ret < 0)
            return ret;

    } else {
        /* when no attrs, digest is contentDigest without tag and length */
        XMEMCPY(esd->contentAttribsDigest, esd->contentDigest + 2, hashSz);
    }

    /* set algoID, with NULL attributes */
    algoIdSz = SetAlgoID(pkcs7->hashOID, algoId, oidHashType, 0);

    digestStrSz = SetOctetString(hashSz, digestStr);
    digestInfoSeqSz = SetSequence(algoIdSz + digestStrSz + hashSz,
                                  digestInfoSeq);

    if (*digestInfoSz < (digestInfoSeqSz + algoIdSz + digestStrSz + hashSz)) {
        return BUFFER_E;
    }

    XMEMCPY(digestInfo + digIdx, digestInfoSeq, digestInfoSeqSz);
    digIdx += digestInfoSeqSz;
    XMEMCPY(digestInfo + digIdx, algoId, algoIdSz);
    digIdx += algoIdSz;
    XMEMCPY(digestInfo + digIdx, digestStr, digestStrSz);
    digIdx += digestStrSz;
    XMEMCPY(digestInfo + digIdx, esd->contentAttribsDigest, hashSz);
    digIdx += hashSz;

    *digestInfoSz = digIdx;

    return 0;
}


/* build SignedData signature over DigestInfo or content digest
 *
 * pkcs7 - pointer to initizlied PKCS7 struct
 * flatSignedAttribs - flattened, signed attributes
 * flatSignedAttribsSz - size of flatSignedAttribs, octets
 * esd - pointer to initialized ESD struct
 *
 * returns length of signature on success, negative on error */
static int wc_PKCS7_SignedDataBuildSignature(PKCS7* pkcs7,
                                             byte* flatSignedAttribs,
                                             word32 flatSignedAttribsSz,
                                             ESD* esd)
{
    int ret;
#ifdef HAVE_ECC
    int hashSz;
#endif
    word32 digestInfoSz = MAX_PKCS7_DIGEST_SZ;
#ifdef WOLFSSL_SMALL_STACK
    byte* digestInfo;
#else
    byte digestInfo[MAX_PKCS7_DIGEST_SZ];
#endif

    if (pkcs7 == NULL || esd == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    digestInfo = (byte*)XMALLOC(digestInfoSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (digestInfo == NULL) {
        return MEMORY_E;
    }
#endif

    ret = wc_PKCS7_BuildDigestInfo(pkcs7, flatSignedAttribs,
                                   flatSignedAttribsSz, esd, digestInfo,
                                   &digestInfoSz);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* sign digestInfo */
    switch (pkcs7->publicKeyOID) {

#ifndef NO_RSA
        case RSAk:
            ret = wc_PKCS7_RsaSign(pkcs7, digestInfo, digestInfoSz, esd);
            break;
#endif

#ifdef HAVE_ECC
        case ECDSAk:
            /* CMS with ECDSA does not sign DigestInfo structure
             * like PKCS#7 with RSA does */
            hashSz = wc_HashGetDigestSize(esd->hashType);
            if (hashSz < 0) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                return hashSz;
            }

            ret = wc_PKCS7_EcdsaSign(pkcs7, esd->contentAttribsDigest,
                                     hashSz, esd);
            break;
#endif

        default:
            WOLFSSL_MSG("Unsupported public key type");
            ret = BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (ret >= 0) {
        esd->encContentDigestSz = (word32)ret;
    }

    return ret;
}

/* build PKCS#7 signedData content type */
int wc_PKCS7_EncodeSignedData(PKCS7* pkcs7, byte* output, word32 outputSz)
{
    static const byte outerOid[] =
        { ASN_OBJECT_ID, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01,
                         0x07, 0x02 };
    static const byte innerOid[] =
        { ASN_OBJECT_ID, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01,
                         0x07, 0x01 };

    byte contentTypeOid[] =
            { ASN_OBJECT_ID, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xF7, 0x0d, 0x01,
                             0x09, 0x03 };
    byte contentType[] =
            { ASN_OBJECT_ID, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
                             0x07, 0x01 };
    byte messageDigestOid[] =
            { ASN_OBJECT_ID, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
                             0x09, 0x04 };

#ifdef WOLFSSL_SMALL_STACK
    ESD* esd = NULL;
#else
    ESD stack_esd;
    ESD* esd = &stack_esd;
#endif

    word32 signerInfoSz = 0;
    word32 totalSz = 0;
    int idx = 0, ret = 0;
    int digEncAlgoId, digEncAlgoType, hashSz;
    byte* flatSignedAttribs = NULL;
    word32 flatSignedAttribsSz = 0;
    word32 innerOidSz = sizeof(innerOid);
    word32 outerOidSz = sizeof(outerOid);

    if (pkcs7 == NULL || pkcs7->content == NULL || pkcs7->contentSz == 0 ||
        pkcs7->encryptOID == 0 || pkcs7->hashOID == 0 || pkcs7->rng == 0 ||
        pkcs7->singleCert == NULL || pkcs7->singleCertSz == 0 ||
        output == NULL || outputSz == 0) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    esd = (ESD*)XMALLOC(sizeof(ESD), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (esd == NULL)
        return MEMORY_E;
#endif

    XMEMSET(esd, 0, sizeof(ESD));

    esd->hashType = wc_OidGetHash(pkcs7->hashOID);
    ret = wc_HashGetDigestSize(esd->hashType);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }
    hashSz = ret;

    ret = wc_HashInit(&esd->hash, esd->hashType);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (pkcs7->contentSz != 0)
    {
        ret = wc_HashUpdate(&esd->hash, esd->hashType,
                            pkcs7->content, pkcs7->contentSz);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
        }
        esd->contentDigest[0] = ASN_OCTET_STRING;
        esd->contentDigest[1] = (byte)hashSz;
        ret = wc_HashFinal(&esd->hash, esd->hashType,
                           &esd->contentDigest[2]);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
        }
    }

    esd->innerOctetsSz = SetOctetString(pkcs7->contentSz, esd->innerOctets);
    esd->innerContSeqSz = SetExplicit(0, esd->innerOctetsSz + pkcs7->contentSz,
                                esd->innerContSeq);
    esd->contentInfoSeqSz = SetSequence(pkcs7->contentSz + esd->innerOctetsSz +
                                    innerOidSz + esd->innerContSeqSz,
                                    esd->contentInfoSeq);

    esd->issuerSnSz = SetSerialNumber(pkcs7->issuerSn, pkcs7->issuerSnSz,
                                     esd->issuerSn, MAX_SN_SZ);
    signerInfoSz += esd->issuerSnSz;
    esd->issuerNameSz = SetSequence(pkcs7->issuerSz, esd->issuerName);
    signerInfoSz += esd->issuerNameSz + pkcs7->issuerSz;
    esd->issuerSnSeqSz = SetSequence(signerInfoSz, esd->issuerSnSeq);
    signerInfoSz += esd->issuerSnSeqSz;
    esd->signerVersionSz = SetMyVersion(1, esd->signerVersion, 0);
    signerInfoSz += esd->signerVersionSz;
    esd->signerDigAlgoIdSz = SetAlgoID(pkcs7->hashOID, esd->signerDigAlgoId,
                                      oidHashType, 0);
    signerInfoSz += esd->signerDigAlgoIdSz;

    /* set signatureAlgorithm */
    ret = wc_PKCS7_SignedDataGetEncAlgoId(pkcs7, &digEncAlgoId,
                                          &digEncAlgoType);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }
    esd->digEncAlgoIdSz = SetAlgoID(digEncAlgoId, esd->digEncAlgoId,
                                    digEncAlgoType, 0);
    signerInfoSz += esd->digEncAlgoIdSz;

    if (pkcs7->signedAttribsSz != 0) {

        /* build up signed attributes */
        ret = wc_PKCS7_BuildSignedAttributes(pkcs7, esd,
                                    contentTypeOid, sizeof(contentTypeOid),
                                    contentType, sizeof(contentType),
                                    messageDigestOid, sizeof(messageDigestOid));
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return MEMORY_E;
        }

        flatSignedAttribs = (byte*)XMALLOC(esd->signedAttribsSz, pkcs7->heap,
                                                         DYNAMIC_TYPE_PKCS7);
        flatSignedAttribsSz = esd->signedAttribsSz;
        if (flatSignedAttribs == NULL) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return MEMORY_E;
        }

        FlattenAttributes(flatSignedAttribs,
                                   esd->signedAttribs, esd->signedAttribsCount);
        esd->signedAttribSetSz = SetImplicit(ASN_SET, 0, esd->signedAttribsSz,
                                                          esd->signedAttribSet);
    }

    /* Calculate the final hash and encrypt it. */
    ret = wc_PKCS7_SignedDataBuildSignature(pkcs7, flatSignedAttribs,
                                            flatSignedAttribsSz, esd);
    if (ret < 0) {
        if (pkcs7->signedAttribsSz != 0)
            XFREE(flatSignedAttribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    signerInfoSz += flatSignedAttribsSz + esd->signedAttribSetSz;

    esd->signerDigestSz = SetOctetString(esd->encContentDigestSz,
                                                             esd->signerDigest);
    signerInfoSz += esd->signerDigestSz + esd->encContentDigestSz;

    esd->signerInfoSeqSz = SetSequence(signerInfoSz, esd->signerInfoSeq);
    signerInfoSz += esd->signerInfoSeqSz;
    esd->signerInfoSetSz = SetSet(signerInfoSz, esd->signerInfoSet);
    signerInfoSz += esd->signerInfoSetSz;

    esd->certsSetSz = SetImplicit(ASN_SET, 0, pkcs7->singleCertSz,
                                                                 esd->certsSet);

    esd->singleDigAlgoIdSz = SetAlgoID(pkcs7->hashOID, esd->singleDigAlgoId,
                                      oidHashType, 0);
    esd->digAlgoIdSetSz = SetSet(esd->singleDigAlgoIdSz, esd->digAlgoIdSet);


    esd->versionSz = SetMyVersion(1, esd->version, 0);

    totalSz = esd->versionSz + esd->singleDigAlgoIdSz + esd->digAlgoIdSetSz +
              esd->contentInfoSeqSz + esd->certsSetSz + pkcs7->singleCertSz +
              esd->innerOctetsSz + esd->innerContSeqSz +
              innerOidSz + pkcs7->contentSz +
              signerInfoSz;
    esd->innerSeqSz = SetSequence(totalSz, esd->innerSeq);
    totalSz += esd->innerSeqSz;
    esd->outerContentSz = SetExplicit(0, totalSz, esd->outerContent);
    totalSz += esd->outerContentSz + outerOidSz;
    esd->outerSeqSz = SetSequence(totalSz, esd->outerSeq);
    totalSz += esd->outerSeqSz;

    if (outputSz < totalSz) {
        if (pkcs7->signedAttribsSz != 0)
            XFREE(flatSignedAttribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BUFFER_E;
    }

    idx = 0;
    XMEMCPY(output + idx, esd->outerSeq, esd->outerSeqSz);
    idx += esd->outerSeqSz;
    XMEMCPY(output + idx, outerOid, outerOidSz);
    idx += outerOidSz;
    XMEMCPY(output + idx, esd->outerContent, esd->outerContentSz);
    idx += esd->outerContentSz;
    XMEMCPY(output + idx, esd->innerSeq, esd->innerSeqSz);
    idx += esd->innerSeqSz;
    XMEMCPY(output + idx, esd->version, esd->versionSz);
    idx += esd->versionSz;
    XMEMCPY(output + idx, esd->digAlgoIdSet, esd->digAlgoIdSetSz);
    idx += esd->digAlgoIdSetSz;
    XMEMCPY(output + idx, esd->singleDigAlgoId, esd->singleDigAlgoIdSz);
    idx += esd->singleDigAlgoIdSz;
    XMEMCPY(output + idx, esd->contentInfoSeq, esd->contentInfoSeqSz);
    idx += esd->contentInfoSeqSz;
    XMEMCPY(output + idx, innerOid, innerOidSz);
    idx += innerOidSz;
    XMEMCPY(output + idx, esd->innerContSeq, esd->innerContSeqSz);
    idx += esd->innerContSeqSz;
    XMEMCPY(output + idx, esd->innerOctets, esd->innerOctetsSz);
    idx += esd->innerOctetsSz;
    XMEMCPY(output + idx, pkcs7->content, pkcs7->contentSz);
    idx += pkcs7->contentSz;
    XMEMCPY(output + idx, esd->certsSet, esd->certsSetSz);
    idx += esd->certsSetSz;
    XMEMCPY(output + idx, pkcs7->singleCert, pkcs7->singleCertSz);
    idx += pkcs7->singleCertSz;
    XMEMCPY(output + idx, esd->signerInfoSet, esd->signerInfoSetSz);
    idx += esd->signerInfoSetSz;
    XMEMCPY(output + idx, esd->signerInfoSeq, esd->signerInfoSeqSz);
    idx += esd->signerInfoSeqSz;
    XMEMCPY(output + idx, esd->signerVersion, esd->signerVersionSz);
    idx += esd->signerVersionSz;
    XMEMCPY(output + idx, esd->issuerSnSeq, esd->issuerSnSeqSz);
    idx += esd->issuerSnSeqSz;
    XMEMCPY(output + idx, esd->issuerName, esd->issuerNameSz);
    idx += esd->issuerNameSz;
    XMEMCPY(output + idx, pkcs7->issuer, pkcs7->issuerSz);
    idx += pkcs7->issuerSz;
    XMEMCPY(output + idx, esd->issuerSn, esd->issuerSnSz);
    idx += esd->issuerSnSz;
    XMEMCPY(output + idx, esd->signerDigAlgoId, esd->signerDigAlgoIdSz);
    idx += esd->signerDigAlgoIdSz;

    /* SignerInfo:Attributes */
    if (flatSignedAttribsSz > 0) {
        XMEMCPY(output + idx, esd->signedAttribSet, esd->signedAttribSetSz);
        idx += esd->signedAttribSetSz;
        XMEMCPY(output + idx, flatSignedAttribs, flatSignedAttribsSz);
        idx += flatSignedAttribsSz;
        XFREE(flatSignedAttribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
    }

    XMEMCPY(output + idx, esd->digEncAlgoId, esd->digEncAlgoIdSz);
    idx += esd->digEncAlgoIdSz;
    XMEMCPY(output + idx, esd->signerDigest, esd->signerDigestSz);
    idx += esd->signerDigestSz;
    XMEMCPY(output + idx, esd->encContentDigest, esd->encContentDigestSz);
    idx += esd->encContentDigestSz;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(esd, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return idx;
}


#ifndef NO_RSA

/* returns size of signature put into out, negative on error */
static int wc_PKCS7_RsaVerify(PKCS7* pkcs7, byte* sig, int sigSz,
                              byte* hash, word32 hashSz)
{
    int ret = 0;
    word32 scratch = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte* digest;
    RsaKey* key;
#else
    byte digest[MAX_PKCS7_DIGEST_SZ];
    RsaKey stack_key;
    RsaKey* key = &stack_key;
#endif

    if (pkcs7 == NULL || sig == NULL || hash == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    digest = (byte*)XMALLOC(MAX_PKCS7_DIGEST_SZ, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);

    if (digest == NULL)
        return MEMORY_E;

    key = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (key == NULL) {
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    XMEMSET(digest, 0, MAX_PKCS7_DIGEST_SZ);

    ret = wc_InitRsaKey_ex(key, pkcs7->heap, pkcs7->devId);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (wc_RsaPublicKeyDecode(pkcs7->publicKey, &scratch, key,
                              pkcs7->publicKeySz) < 0) {
        WOLFSSL_MSG("ASN RSA key decode error");
        wc_FreeRsaKey(key);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return PUBLIC_KEY_E;
    }

    ret = wc_RsaSSL_Verify(sig, sigSz, digest, MAX_PKCS7_DIGEST_SZ, key);

    wc_FreeRsaKey(key);

    if (((int)hashSz != ret) || (XMEMCMP(digest, hash, ret) != 0)) {
        ret = SIG_VERIFY_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* NO_RSA */


#ifdef HAVE_ECC

/* returns size of signature put into out, negative on error */
static int wc_PKCS7_EcdsaVerify(PKCS7* pkcs7, byte* sig, int sigSz,
                                byte* hash, word32 hashSz)
{
    int ret = 0;
    int res = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte* digest;
    ecc_key* key;
#else
    byte digest[MAX_PKCS7_DIGEST_SZ];
    ecc_key stack_key;
    ecc_key* key = &stack_key;
#endif
    word32 idx = 0;

    if (pkcs7 == NULL || sig == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    digest = (byte*)XMALLOC(MAX_PKCS7_DIGEST_SZ, NULL,
                            DYNAMIC_TYPE_TMP_BUFFER);

    if (digest == NULL)
        return MEMORY_E;

    key = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (key == NULL) {
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    XMEMSET(digest, 0, MAX_PKCS7_DIGEST_SZ);

    ret = wc_ecc_init_ex(key, pkcs7->heap, pkcs7->devId);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (wc_EccPublicKeyDecode(pkcs7->publicKey, &idx, key,
                                                      pkcs7->publicKeySz) < 0) {
        WOLFSSL_MSG("ASN ECDSA key decode error");
        wc_ecc_free(key);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return PUBLIC_KEY_E;
    }

    ret = wc_ecc_verify_hash(sig, sigSz, hash, hashSz, &res, key);

    wc_ecc_free(key);

    if (ret == 0 && res != 1) {
        ret = SIG_VERIFY_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(key,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif /* HAVE_ECC */


/* build SignedData digest, both in PKCS#7 DigestInfo format and
 * as plain digest for CMS.
 *
 * pkcs7          - pointer to initialized PKCS7 struct
 * signedAttrib   - signed attributes
 * signedAttribSz - size of signedAttrib, octets
 * pkcs7Digest    - [OUT] PKCS#7 DigestInfo
 * pkcs7DigestSz  - [IN/OUT] size of pkcs7Digest
 * plainDigest    - [OUT] pointer to plain digest, offset into pkcs7Digest
 * plainDigestSz  - [OUT] size of digest at plainDigest
 *
 * returns 0 on success, negative on error */
static int wc_PKCS7_BuildSignedDataDigest(PKCS7* pkcs7, byte* signedAttrib,
                                      word32 signedAttribSz, byte* pkcs7Digest,
                                      word32* pkcs7DigestSz, byte** plainDigest,
                                      word32* plainDigestSz)
{
    int ret = 0, digIdx = 0, hashSz;
    word32 attribSetSz;
    byte attribSet[MAX_SET_SZ];
    byte digest[WC_MAX_DIGEST_SIZE];
    byte digestInfoSeq[MAX_SEQ_SZ];
    byte digestStr[MAX_OCTET_STR_SZ];
    byte algoId[MAX_ALGO_SZ];
    word32 digestInfoSeqSz, digestStrSz, algoIdSz;
#ifdef WOLFSSL_SMALL_STACK
    byte* digestInfo;
#else
    byte digestInfo[MAX_PKCS7_DIGEST_SZ];
#endif

    wc_HashAlg hash;
    enum wc_HashType hashType;

    if (pkcs7 == NULL || pkcs7Digest == NULL ||
        pkcs7DigestSz == NULL || plainDigest == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    digestInfo = (byte*)XMALLOC(MAX_PKCS7_DIGEST_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (digestInfo == NULL)
        return MEMORY_E;
#endif

    XMEMSET(pkcs7Digest, 0, *pkcs7DigestSz);
    XMEMSET(digest,      0, WC_MAX_DIGEST_SIZE);
    XMEMSET(digestInfo,  0, MAX_PKCS7_DIGEST_SZ);

    hashType = wc_OidGetHash(pkcs7->hashOID);
    ret = wc_HashGetDigestSize(hashType);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }
    hashSz = ret;

    /* calculate digest */
    ret = wc_HashInit(&hash, hashType);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (signedAttribSz > 0) {

        if (signedAttrib == NULL) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return BAD_FUNC_ARG;
        }

        attribSetSz = SetSet(signedAttribSz, attribSet);
        ret = wc_HashUpdate(&hash, hashType, attribSet, attribSetSz);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ret;
        }

        ret = wc_HashUpdate(&hash, hashType, signedAttrib, signedAttribSz);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ret;
        }

        ret = wc_HashFinal(&hash, hashType, digest);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ret;
        }

    } else {

        if (pkcs7->content == NULL) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return BAD_FUNC_ARG;
        }

        ret = wc_HashUpdate(&hash, hashType, pkcs7->content, pkcs7->contentSz);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ret;
        }

        ret = wc_HashFinal(&hash, hashType, digest);
        if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ret;
        }
    }

    /* Set algoID, with NULL attributes */
    algoIdSz = SetAlgoID(pkcs7->hashOID, algoId, oidHashType, 0);

    digestStrSz = SetOctetString(hashSz, digestStr);
    digestInfoSeqSz = SetSequence(algoIdSz + digestStrSz + hashSz,
                                  digestInfoSeq);

    XMEMCPY(digestInfo + digIdx, digestInfoSeq, digestInfoSeqSz);
    digIdx += digestInfoSeqSz;
    XMEMCPY(digestInfo + digIdx, algoId, algoIdSz);
    digIdx += algoIdSz;
    XMEMCPY(digestInfo + digIdx, digestStr, digestStrSz);
    digIdx += digestStrSz;
    XMEMCPY(digestInfo + digIdx, digest, hashSz);
    digIdx += hashSz;

    XMEMCPY(pkcs7Digest, digestInfo, digIdx);
    *pkcs7DigestSz = digIdx;

    /* set plain digest pointer */
    *plainDigest = pkcs7Digest + digIdx - hashSz;
    *plainDigestSz = hashSz;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(digestInfo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return 0;
}


/* verifies SignedData signature, over either PKCS#7 DigestInfo or
 * content digest.
 *
 * pkcs7          - pointer to initialized PKCS7 struct
 * sig            - signature to verify
 * sigSz          - size of sig
 * signedAttrib   - signed attributes, or null if empty
 * signedAttribSz - size of signedAttributes
 *
 * return 0 on success, negative on error */
static int wc_PKCS7_SignedDataVerifySignature(PKCS7* pkcs7, byte* sig,
                                              word32 sigSz, byte* signedAttrib,
                                              word32 signedAttribSz)
{
    int ret = 0;
    word32 plainDigestSz = 0, pkcs7DigestSz;
    byte* plainDigest = NULL; /* offset into pkcs7Digest */
#ifdef WOLFSSL_SMALL_STACK
    byte* pkcs7Digest;
#else
    byte  pkcs7Digest[MAX_PKCS7_DIGEST_SZ];
#endif

    if (pkcs7 == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    pkcs7Digest = (byte*)XMALLOC(MAX_PKCS7_DIGEST_SZ, NULL,
                                 DYNAMIC_TYPE_TMP_BUFFER);
    if (pkcs7Digest == NULL)
        return MEMORY_E;
#endif

    /* build hash to verify against */
    pkcs7DigestSz = MAX_PKCS7_DIGEST_SZ;
    ret = wc_PKCS7_BuildSignedDataDigest(pkcs7, signedAttrib,
                                         signedAttribSz, pkcs7Digest,
                                         &pkcs7DigestSz, &plainDigest,
                                         &plainDigestSz);
    if (ret < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(pkcs7Digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    switch (pkcs7->publicKeyOID) {

#ifndef NO_RSA
        case RSAk:
            ret = wc_PKCS7_RsaVerify(pkcs7, sig, sigSz, pkcs7Digest,
                                     pkcs7DigestSz);
            if (ret < 0) {
                WOLFSSL_MSG("PKCS#7 verification failed, trying CMS");
                ret = wc_PKCS7_RsaVerify(pkcs7, sig, sigSz, plainDigest,
                                         plainDigestSz);
            }
            break;
#endif

#ifdef HAVE_ECC
        case ECDSAk:
            ret = wc_PKCS7_EcdsaVerify(pkcs7, sig, sigSz, plainDigest,
                                       plainDigestSz);
            break;
#endif

        default:
            WOLFSSL_MSG("Unsupported public key type");
            ret = BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
     XFREE(pkcs7Digest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return ret;
}


/* set correct public key OID based on signature OID, stores in
 * pkcs7->publicKeyOID and returns same value */
static int wc_PKCS7_SetPublicKeyOID(PKCS7* pkcs7, int sigOID)
{
    if (pkcs7 == NULL)
        return BAD_FUNC_ARG;

    pkcs7->publicKeyOID = 0;

    switch (sigOID) {

    #ifndef NO_RSA
        /* RSA signature types */
        case CTC_MD2wRSA:
        case CTC_MD5wRSA:
        case CTC_SHAwRSA:
        case CTC_SHA224wRSA:
        case CTC_SHA256wRSA:
        case CTC_SHA384wRSA:
        case CTC_SHA512wRSA:
            pkcs7->publicKeyOID = RSAk;
            break;

        /* if sigOID is already RSAk */
        case RSAk:
            pkcs7->publicKeyOID = sigOID;
            break;
    #endif

    #ifndef NO_DSA
        /* DSA signature types */
        case CTC_SHAwDSA:
            pkcs7->publicKeyOID = DSAk;
            break;

        /* if sigOID is already DSAk */
        case DSAk:
            pkcs7->publicKeyOID = sigOID;
            break;
    #endif

    #ifdef HAVE_ECC
        /* ECDSA signature types */
        case CTC_SHAwECDSA:
        case CTC_SHA224wECDSA:
        case CTC_SHA256wECDSA:
        case CTC_SHA384wECDSA:
        case CTC_SHA512wECDSA:
            pkcs7->publicKeyOID = ECDSAk;
            break;

        /* if sigOID is already ECDSAk */
        case ECDSAk:
            pkcs7->publicKeyOID = sigOID;
            break;
    #endif

        default:
            WOLFSSL_MSG("Unsupported public key algorithm");
            return ASN_SIG_KEY_E;
    }

    return pkcs7->publicKeyOID;
}


/* Parses through the attributes and adds them to the PKCS7 structure
 * Creates dynamic attribute structures that are free'd with calling
 * wc_PKCS7_Free()
 *
 * NOTE: An attribute has the ASN1 format of
 ** Sequence
 ****** Object ID
 ****** Set
 ********** {PritnableString, UTCTime, OCTET STRING ...}
 *
 * pkcs7  the PKCS7 structure to put the parsed attributes into
 * in     buffer holding all attributes
 * inSz   size of in buffer
 *
 * returns the number of attributes parsed on success
 */
static int wc_PKCS7_ParseAttribs(PKCS7* pkcs7, byte* in, int inSz)
{
    int    found = 0;
    word32 idx   = 0;
    word32 oid;

    if (pkcs7 == NULL || in == NULL || inSz < 0) {
        return BAD_FUNC_ARG;
    }

    while (idx < (word32)inSz) {
        int length  = 0;
        int oidIdx;
        PKCS7DecodedAttrib* attrib;

        if (GetSequence(in, &idx, &length, inSz) < 0)
            return ASN_PARSE_E;

        attrib = (PKCS7DecodedAttrib*)XMALLOC(sizeof(PKCS7DecodedAttrib),
                pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        if (attrib == NULL) {
            return MEMORY_E;
        }
        XMEMSET(attrib, 0, sizeof(PKCS7DecodedAttrib));

        oidIdx = idx;
        if (GetObjectId(in, &idx, &oid, oidIgnoreType, inSz)
                < 0) {
            XFREE(attrib, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return ASN_PARSE_E;
        }
        attrib->oidSz = idx - oidIdx;
        attrib->oid = (byte*)XMALLOC(attrib->oidSz, pkcs7->heap,
                                     DYNAMIC_TYPE_PKCS7);
        if (attrib->oid == NULL) {
            XFREE(attrib, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return MEMORY_E;
        }
        XMEMCPY(attrib->oid, in + oidIdx, attrib->oidSz);


        /* Get Set that contains the printable string value */
        if (GetSet(in, &idx, &length, inSz) < 0) {
            XFREE(attrib->oid, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(attrib, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return ASN_PARSE_E;
        }

        if ((inSz - idx) < (word32)length) {
            XFREE(attrib->oid, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(attrib, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return ASN_PARSE_E;
        }

        attrib->valueSz = (word32)length;
        attrib->value = (byte*)XMALLOC(attrib->valueSz, pkcs7->heap,
                                       DYNAMIC_TYPE_PKCS7);
        if (attrib->value == NULL) {
            XFREE(attrib->oid, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(attrib, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return MEMORY_E;
        }
        XMEMCPY(attrib->value, in + idx, attrib->valueSz);
        idx += length;

        /* store attribute in linked list */
        if (pkcs7->decodedAttrib != NULL) {
            attrib->next = pkcs7->decodedAttrib;
            pkcs7->decodedAttrib = attrib;
        } else {
            pkcs7->decodedAttrib = attrib;
        }
        found++;
    }

    return found;
}


/* Finds the certificates in the message and saves it. */
int wc_PKCS7_VerifySignedData(PKCS7* pkcs7, byte* pkiMsg, word32 pkiMsgSz)
{
    word32 idx, contentType, hashOID, sigOID;
    int length, version, ret;
    byte* content = NULL;
    byte* sig = NULL;
    byte* cert = NULL;
    byte* signedAttrib = NULL;
    int contentSz = 0, sigSz = 0, certSz = 0, signedAttribSz = 0;
    byte degenerate;
#ifdef ASN_BER_TO_DER
    byte* der;
#endif

    if (pkcs7 == NULL || pkiMsg == NULL || pkiMsgSz == 0)
        return BAD_FUNC_ARG;

    idx = 0;

    /* Get the contentInfo sequence */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (length == 0 && pkiMsg[idx-1] == 0x80) {
#ifdef ASN_BER_TO_DER
        word32 len = 0;

        ret = wc_BerToDer(pkiMsg, pkiMsgSz, NULL, &len);
        if (ret != LENGTH_ONLY_E)
            return ret;
        pkcs7->der = (byte*)XMALLOC(len, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        if (pkcs7->der == NULL)
            return MEMORY_E;
        ret = wc_BerToDer(pkiMsg, pkiMsgSz, pkcs7->der, &len);
        if (ret < 0)
            return ret;

        pkiMsg = pkcs7->der;
        pkiMsgSz = len;
        idx = 0;
        if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;
#else
        return BER_INDEF_E;
#endif
    }

    /* Get the contentInfo contentType */
    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (contentType != SIGNED_DATA) {
        WOLFSSL_MSG("PKCS#7 input not of type SignedData");
        return PKCS7_OID_E;
    }

    /* get the ContentInfo content */
    if (pkiMsg[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0))
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* Get the signedData sequence */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* Get the version */
    if (GetMyVersion(pkiMsg, &idx, &version, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (version != 1) {
        WOLFSSL_MSG("PKCS#7 signedData needs to be of version 1");
        return ASN_VERSION_E;
    }

    /* Get the set of DigestAlgorithmIdentifiers */
    if (GetSet(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* Skip the set. */
    idx += length;
    degenerate = (length == 0)? 1 : 0;

    /* Get the inner ContentInfo sequence */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* Get the inner ContentInfo contentType */
    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (contentType != DATA) {
        WOLFSSL_MSG("PKCS#7 inner input not of type Data");
        return PKCS7_OID_E;
    }

    /* Check for content info, it could be omitted when degenerate */
    {
        word32 localIdx = idx;
        ret = 0;
        if (pkiMsg[localIdx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0))
            ret = ASN_PARSE_E;

        if (ret == 0 && GetLength(pkiMsg, &localIdx, &length, pkiMsgSz) <= 0)
            ret = ASN_PARSE_E;

        if (ret == 0 && pkiMsg[localIdx++] != ASN_OCTET_STRING)
            ret = ASN_PARSE_E;

        if (ret == 0 && GetLength(pkiMsg, &localIdx, &length, pkiMsgSz) < 0)
            ret = ASN_PARSE_E;

        /* Save the inner data as the content. */
        if (length > 0) {
            /* Local pointer for calculating hashes later */
            content   = &pkiMsg[localIdx];
            contentSz = length;
            localIdx += length;
        }

        /* update idx if successful */
        if (ret == 0) {
            idx = localIdx;
        }
    }

    /* If getting the content info failed with non degenerate then return the
     * error case. Otherwise with a degenerate it is ok if the content
     * info was omitted */
    if (!degenerate && ret != 0) {
        return ret;
    }

    /* Get the implicit[0] set of certificates */
    if (pkiMsg[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)) {
        idx++;
        if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

        if (length > 0) {
            /* At this point, idx is at the first certificate in
             * a set of certificates. There may be more than one,
             * or none, or they may be a PKCS 6 extended
             * certificate. We want to save the first cert if it
             * is X.509. */

            word32 certIdx = idx;

            if (pkiMsg[certIdx++] == (ASN_CONSTRUCTED | ASN_SEQUENCE)) {
                if (GetLength(pkiMsg, &certIdx, &certSz, pkiMsgSz) < 0)
                    return ASN_PARSE_E;

                cert = &pkiMsg[idx];
                certSz += (certIdx - idx);
            }

#ifdef ASN_BER_TO_DER
            der = pkcs7->der;
#endif
            /* This will reset PKCS7 structure and then set the certificate */
            wc_PKCS7_InitWithCert(pkcs7, cert, certSz);
#ifdef ASN_BER_TO_DER
            pkcs7->der = der;
#endif

            /* iterate through any additional certificates */
            if (MAX_PKCS7_CERTS > 0) {
                word32 localIdx;
                int sz = 0;
                int i;

                pkcs7->cert[0]   = cert;
                pkcs7->certSz[0] = certSz;
                certIdx = idx + certSz;

                for (i = 1; i < MAX_PKCS7_CERTS && certIdx + 1 < pkiMsgSz; i++) {
                    localIdx = certIdx;

                    if (pkiMsg[certIdx++] == (ASN_CONSTRUCTED | ASN_SEQUENCE)) {
                        if (GetLength(pkiMsg, &certIdx, &sz, pkiMsgSz) < 0)
                            return ASN_PARSE_E;

                        pkcs7->cert[i]   = &pkiMsg[localIdx];
                        pkcs7->certSz[i] = sz + (certIdx - localIdx);
                        certIdx += sz;
                    }
                }
            }
        }
        idx += length;
    }

    /* set content and size after init of PKCS7 structure */
    pkcs7->content   = content;
    pkcs7->contentSz = contentSz;

    /* Get the implicit[1] set of crls */
    if (pkiMsg[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1)) {
        idx++;
        if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

        /* Skip the set */
        idx += length;
    }

    /* Get the set of signerInfos */
    if (GetSet(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (length > 0) {
        /* Get the sequence of the first signerInfo */
        if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

        /* Get the version */
        if (GetMyVersion(pkiMsg, &idx, &version, pkiMsgSz) < 0)
            return ASN_PARSE_E;

        if (version != 1) {
            WOLFSSL_MSG("PKCS#7 signerInfo needs to be of version 1");
            return ASN_VERSION_E;
        }

        /* Get the sequence of IssuerAndSerialNumber */
        if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

        /* Skip it */
        idx += length;

        /* Get the sequence of digestAlgorithm */
        if (GetAlgoId(pkiMsg, &idx, &hashOID, oidHashType, pkiMsgSz) < 0) {
            return ASN_PARSE_E;
        }
        pkcs7->hashOID = (int)hashOID;

        /* Get the IMPLICIT[0] SET OF signedAttributes */
        if (pkiMsg[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)) {
            idx++;

            if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
                return ASN_PARSE_E;

            /* save pointer and length */
            signedAttrib = &pkiMsg[idx];
            signedAttribSz = length;

            if (wc_PKCS7_ParseAttribs(pkcs7, signedAttrib, signedAttribSz) <0) {
                WOLFSSL_MSG("Error parsing signed attributes");
                return ASN_PARSE_E;
            }

            idx += length;
        }

        /* Get digestEncryptionAlgorithm */
        if (GetAlgoId(pkiMsg, &idx, &sigOID, oidSigType, pkiMsgSz) < 0) {
            return ASN_PARSE_E;
        }

        /* store public key type based on digestEncryptionAlgorithm */
        ret = wc_PKCS7_SetPublicKeyOID(pkcs7, sigOID);
        if (ret <= 0) {
            WOLFSSL_MSG("Failed to set public key OID from signature");
            return ret;
        }

        /* Get the signature */
        if (pkiMsg[idx] == ASN_OCTET_STRING) {
            idx++;

            if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
                return ASN_PARSE_E;

            /* save pointer and length */
            sig = &pkiMsg[idx];
            sigSz = length;

            idx += length;
        }

        pkcs7->content = content;
        pkcs7->contentSz = contentSz;

        ret = wc_PKCS7_SignedDataVerifySignature(pkcs7, sig, sigSz,
                                                 signedAttrib, signedAttribSz);
        if (ret < 0)
            return ret;
    }

    return 0;
}


#ifdef HAVE_ECC

/* KARI == KeyAgreeRecipientInfo (key agreement) */
typedef struct WC_PKCS7_KARI {
    DecodedCert* decoded;          /* decoded recip cert */
    void*    heap;                 /* user heap, points to PKCS7->heap */
    int      devId;                /* device ID for HW based private key */
    ecc_key* recipKey;             /* recip key  (pub | priv) */
    ecc_key* senderKey;            /* sender key (pub | priv) */
    byte*    senderKeyExport;      /* sender ephemeral key DER */
    byte*    kek;                  /* key encryption key */
    byte*    ukm;                  /* OPTIONAL user keying material */
    byte*    sharedInfo;           /* ECC-CMS-SharedInfo ASN.1 encoded blob */
    word32   senderKeyExportSz;    /* size of sender ephemeral key DER */
    word32   kekSz;                /* size of key encryption key */
    word32   ukmSz;                /* size of user keying material */
    word32   sharedInfoSz;         /* size of ECC-CMS-SharedInfo encoded */
    byte     ukmOwner;             /* do we own ukm buffer? 1:yes, 0:no */
    byte     direction;            /* WC_PKCS7_ENCODE | WC_PKCS7_DECODE */
    byte     decodedInit : 1;      /* indicates decoded was initialized */
    byte     recipKeyInit : 1;     /* indicates recipKey was initialized */
    byte     senderKeyInit : 1;    /* indicates senderKey was initialized */
} WC_PKCS7_KARI;


/* wrap CEK (content encryption key) with KEK, 0 on success, < 0 on error */
static int wc_PKCS7_KariKeyWrap(byte* cek, word32 cekSz, byte* kek,
                                word32 kekSz, byte* out, word32 outSz,
                                int keyWrapAlgo, int direction)
{
    int ret;

    if (cek == NULL || kek == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (keyWrapAlgo) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128_WRAP:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192_WRAP:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256_WRAP:
    #endif

            if (direction == AES_ENCRYPTION) {

                ret = wc_AesKeyWrap(kek, kekSz, cek, cekSz,
                                    out, outSz, NULL);

            } else if (direction == AES_DECRYPTION) {

                ret = wc_AesKeyUnWrap(kek, kekSz, cek, cekSz,
                                      out, outSz, NULL);
            } else {
                WOLFSSL_MSG("Bad key un/wrap direction");
                return BAD_FUNC_ARG;
            }

            if (ret <= 0)
                return ret;

            break;
#endif /* NO_AES */

        default:
            WOLFSSL_MSG("Unsupported key wrap algorithm");
            return BAD_KEYWRAP_ALG_E;
    };

    (void)cekSz;
    (void)kekSz;
    (void)outSz;
    (void)direction;
    return ret;
}


/* allocate and create new WC_PKCS7_KARI struct,
 * returns struct pointer on success, NULL on failure */
static WC_PKCS7_KARI* wc_PKCS7_KariNew(PKCS7* pkcs7, byte direction)
{
    WC_PKCS7_KARI* kari = NULL;

    if (pkcs7 == NULL)
        return NULL;

    kari = (WC_PKCS7_KARI*)XMALLOC(sizeof(WC_PKCS7_KARI), pkcs7->heap,
                                   DYNAMIC_TYPE_PKCS7);
    if (kari == NULL) {
        WOLFSSL_MSG("Failed to allocate WC_PKCS7_KARI");
        return NULL;
    }

    kari->decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert), pkcs7->heap,
                                          DYNAMIC_TYPE_PKCS7);
    if (kari->decoded == NULL) {
        WOLFSSL_MSG("Failed to allocate DecodedCert");
        XFREE(kari, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return NULL;
    }

    kari->recipKey = (ecc_key*)XMALLOC(sizeof(ecc_key), pkcs7->heap,
                                       DYNAMIC_TYPE_PKCS7);
    if (kari->recipKey == NULL) {
        WOLFSSL_MSG("Failed to allocate recipient ecc_key");
        XFREE(kari->decoded, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(kari, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return NULL;
    }

    kari->senderKey = (ecc_key*)XMALLOC(sizeof(ecc_key), pkcs7->heap,
                                        DYNAMIC_TYPE_PKCS7);
    if (kari->senderKey == NULL) {
        WOLFSSL_MSG("Failed to allocate sender ecc_key");
        XFREE(kari->recipKey, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(kari->decoded, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(kari, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return NULL;
    }

    kari->senderKeyExport = NULL;
    kari->senderKeyExportSz = 0;
    kari->kek = NULL;
    kari->kekSz = 0;
    kari->ukm = NULL;
    kari->ukmSz = 0;
    kari->ukmOwner = 0;
    kari->sharedInfo = NULL;
    kari->sharedInfoSz = 0;
    kari->direction = direction;
    kari->decodedInit = 0;
    kari->recipKeyInit = 0;
    kari->senderKeyInit = 0;

    kari->heap = pkcs7->heap;
    kari->devId = pkcs7->devId;

    return kari;
}


/* free WC_PKCS7_KARI struct, return 0 on success */
static int wc_PKCS7_KariFree(WC_PKCS7_KARI* kari)
{
    void* heap;

    if (kari) {
        heap = kari->heap;

        if (kari->decoded) {
            if (kari->decodedInit)
                FreeDecodedCert(kari->decoded);
            XFREE(kari->decoded, heap, DYNAMIC_TYPE_PKCS7);
        }
        if (kari->senderKey) {
            if (kari->senderKeyInit)
                wc_ecc_free(kari->senderKey);
            XFREE(kari->senderKey, heap, DYNAMIC_TYPE_PKCS7);
        }
        if (kari->recipKey) {
            if (kari->recipKeyInit)
                wc_ecc_free(kari->recipKey);
            XFREE(kari->recipKey, heap, DYNAMIC_TYPE_PKCS7);
        }
        if (kari->senderKeyExport) {
            ForceZero(kari->senderKeyExport, kari->senderKeyExportSz);
            XFREE(kari->senderKeyExport, heap, DYNAMIC_TYPE_PKCS7);
            kari->senderKeyExportSz = 0;
        }
        if (kari->kek) {
            ForceZero(kari->kek, kari->kekSz);
            XFREE(kari->kek, heap, DYNAMIC_TYPE_PKCS7);
            kari->kekSz = 0;
        }
        if (kari->ukm) {
            if (kari->ukmOwner == 1) {
                XFREE(kari->ukm, heap, DYNAMIC_TYPE_PKCS7);
            }
            kari->ukmSz = 0;
        }
        if (kari->sharedInfo) {
            ForceZero(kari->sharedInfo, kari->sharedInfoSz);
            XFREE(kari->sharedInfo, heap, DYNAMIC_TYPE_PKCS7);
            kari->sharedInfoSz = 0;
        }
        XFREE(kari, heap, DYNAMIC_TYPE_PKCS7);
    }

    (void)heap;

    return 0;
}


/* parse recipient cert/key, return 0 on success, negative on error
 * key/keySz only needed during decoding (WC_PKCS7_DECODE) */
static int wc_PKCS7_KariParseRecipCert(WC_PKCS7_KARI* kari, const byte* cert,
                                       word32 certSz, const byte* key,
                                       word32 keySz)
{
    int ret;
    word32 idx;

    if (kari == NULL || kari->decoded == NULL ||
        cert == NULL || certSz == 0)
        return BAD_FUNC_ARG;

    /* decode certificate */
    InitDecodedCert(kari->decoded, (byte*)cert, certSz, kari->heap);
    kari->decodedInit = 1;
    ret = ParseCert(kari->decoded, CA_TYPE, NO_VERIFY, 0);
    if (ret < 0)
        return ret;

    /* make sure subject key id was read from cert */
    if (kari->decoded->extSubjKeyIdSet == 0) {
        WOLFSSL_MSG("Failed to read subject key ID from recipient cert");
        return BAD_FUNC_ARG;
    }

    ret = wc_ecc_init_ex(kari->recipKey, kari->heap, kari->devId);
    if (ret != 0)
        return ret;

    kari->recipKeyInit = 1;

    /* get recip public key */
    if (kari->direction == WC_PKCS7_ENCODE) {

        idx = 0;
        ret = wc_EccPublicKeyDecode(kari->decoded->publicKey, &idx,
                                    kari->recipKey, kari->decoded->pubKeySize);
        if (ret != 0)
            return ret;
    }
    /* get recip private key */
    else if (kari->direction == WC_PKCS7_DECODE) {
        if (key != NULL && keySz > 0) {
            idx = 0;
            ret = wc_EccPrivateKeyDecode(key, &idx, kari->recipKey, keySz);
        }
        else if (kari->devId == INVALID_DEVID) {
            ret = BAD_FUNC_ARG;
        }
        if (ret != 0)
            return ret;

    } else {
        /* bad direction */
        return BAD_FUNC_ARG;
    }

    (void)idx;

    return 0;
}


/* create ephemeral ECC key, places ecc_key in kari->senderKey,
 * DER encoded in kari->senderKeyExport. return 0 on success,
 * negative on error */
static int wc_PKCS7_KariGenerateEphemeralKey(WC_PKCS7_KARI* kari, WC_RNG* rng)
{
    int ret;

    if (kari == NULL || kari->decoded == NULL ||
        kari->recipKey == NULL || kari->recipKey->dp == NULL ||
        rng == NULL)
        return BAD_FUNC_ARG;

    kari->senderKeyExport = (byte*)XMALLOC(kari->decoded->pubKeySize, kari->heap,
                                           DYNAMIC_TYPE_PKCS7);
    if (kari->senderKeyExport == NULL)
        return MEMORY_E;

    kari->senderKeyExportSz = kari->decoded->pubKeySize;

    ret = wc_ecc_init_ex(kari->senderKey, kari->heap, kari->devId);
    if (ret != 0)
        return ret;

    kari->senderKeyInit = 1;

    ret = wc_ecc_make_key_ex(rng, kari->recipKey->dp->size,
                             kari->senderKey, kari->recipKey->dp->id);
    if (ret != 0)
        return ret;

    /* dump generated key to X.963 DER for output in CMS bundle */
    ret = wc_ecc_export_x963(kari->senderKey, kari->senderKeyExport,
                             &kari->senderKeyExportSz);
    if (ret != 0)
        return ret;

    return 0;
}


/* create ASN.1 encoded ECC-CMS-SharedInfo using specified key wrap algorithm,
 * place in kari->sharedInfo. returns 0 on success, negative on error */
static int wc_PKCS7_KariGenerateSharedInfo(WC_PKCS7_KARI* kari, int keyWrapOID)
{
    int idx = 0;
    int sharedInfoSeqSz = 0;
    int keyInfoSz = 0;
    int suppPubInfoSeqSz = 0;
    int entityUInfoOctetSz = 0;
    int entityUInfoExplicitSz = 0;
    int kekOctetSz = 0;
    int sharedInfoSz = 0;

    word32 kekBitSz = 0;

    byte sharedInfoSeq[MAX_SEQ_SZ];
    byte keyInfo[MAX_ALGO_SZ];
    byte suppPubInfoSeq[MAX_SEQ_SZ];
    byte entityUInfoOctet[MAX_OCTET_STR_SZ];
    byte entityUInfoExplicitSeq[MAX_SEQ_SZ];
    byte kekOctet[MAX_OCTET_STR_SZ];

    if (kari == NULL)
        return BAD_FUNC_ARG;

    if ((kari->ukmSz > 0) && (kari->ukm == NULL))
        return BAD_FUNC_ARG;

    /* kekOctet */
    kekOctetSz = SetOctetString(sizeof(word32), kekOctet);
    sharedInfoSz += (kekOctetSz + sizeof(word32));

    /* suppPubInfo */
    suppPubInfoSeqSz = SetImplicit(ASN_SEQUENCE, 2,
                                   kekOctetSz + sizeof(word32),
                                   suppPubInfoSeq);
    sharedInfoSz += suppPubInfoSeqSz;

    /* optional ukm/entityInfo */
    if (kari->ukmSz > 0) {
        entityUInfoOctetSz = SetOctetString(kari->ukmSz, entityUInfoOctet);
        sharedInfoSz += (entityUInfoOctetSz + kari->ukmSz);

        entityUInfoExplicitSz = SetExplicit(0, entityUInfoOctetSz +
                                            kari->ukmSz,
                                            entityUInfoExplicitSeq);
        sharedInfoSz += entityUInfoExplicitSz;
    }

    /* keyInfo */
    keyInfoSz = SetAlgoID(keyWrapOID, keyInfo, oidKeyWrapType, 0);
    sharedInfoSz += keyInfoSz;

    /* sharedInfo */
    sharedInfoSeqSz = SetSequence(sharedInfoSz, sharedInfoSeq);
    sharedInfoSz += sharedInfoSeqSz;

    kari->sharedInfo = (byte*)XMALLOC(sharedInfoSz, kari->heap,
                                      DYNAMIC_TYPE_PKCS7);
    if (kari->sharedInfo == NULL)
        return MEMORY_E;

    kari->sharedInfoSz = sharedInfoSz;

    XMEMCPY(kari->sharedInfo + idx, sharedInfoSeq, sharedInfoSeqSz);
    idx += sharedInfoSeqSz;
    XMEMCPY(kari->sharedInfo + idx, keyInfo, keyInfoSz);
    idx += keyInfoSz;
    if (kari->ukmSz > 0) {
        XMEMCPY(kari->sharedInfo + idx, entityUInfoExplicitSeq,
                entityUInfoExplicitSz);
        idx += entityUInfoExplicitSz;
        XMEMCPY(kari->sharedInfo + idx, entityUInfoOctet, entityUInfoOctetSz);
        idx += entityUInfoOctetSz;
        XMEMCPY(kari->sharedInfo + idx, kari->ukm, kari->ukmSz);
        idx += kari->ukmSz;
    }
    XMEMCPY(kari->sharedInfo + idx, suppPubInfoSeq, suppPubInfoSeqSz);
    idx += suppPubInfoSeqSz;
    XMEMCPY(kari->sharedInfo + idx, kekOctet, kekOctetSz);
    idx += kekOctetSz;

    kekBitSz = (kari->kekSz) * 8;              /* convert to bits */
#ifdef LITTLE_ENDIAN_ORDER
    kekBitSz = ByteReverseWord32(kekBitSz);    /* network byte order */
#endif
    XMEMCPY(kari->sharedInfo + idx, &kekBitSz, sizeof(kekBitSz));

    return 0;
}


/* create key encryption key (KEK) using key wrap algorithm and key encryption
 * algorithm, place in kari->kek. return 0 on success, <0 on error. */
static int wc_PKCS7_KariGenerateKEK(WC_PKCS7_KARI* kari,
                                    int keyWrapOID, int keyEncOID)
{
    int ret;
    int kSz;
    enum wc_HashType kdfType;
    byte*  secret;
    word32 secretSz;

    if (kari == NULL || kari->recipKey == NULL ||
        kari->senderKey == NULL || kari->senderKey->dp == NULL)
        return BAD_FUNC_ARG;

    /* get KEK size, allocate buff */
    kSz = wc_PKCS7_GetOIDKeySize(keyWrapOID);
    if (kSz < 0)
        return kSz;

    kari->kek = (byte*)XMALLOC(kSz, kari->heap, DYNAMIC_TYPE_PKCS7);
    if (kari->kek == NULL)
        return MEMORY_E;

    kari->kekSz = (word32)kSz;

    /* generate ECC-CMS-SharedInfo */
    ret = wc_PKCS7_KariGenerateSharedInfo(kari, keyWrapOID);
    if (ret != 0)
        return ret;

    /* generate shared secret */
    secretSz = kari->senderKey->dp->size;
    secret = (byte*)XMALLOC(secretSz, kari->heap, DYNAMIC_TYPE_PKCS7);
    if (secret == NULL)
        return MEMORY_E;

    if (kari->direction == WC_PKCS7_ENCODE) {

        ret = wc_ecc_shared_secret(kari->senderKey, kari->recipKey,
                                   secret, &secretSz);

    } else if (kari->direction == WC_PKCS7_DECODE) {

        ret = wc_ecc_shared_secret(kari->recipKey, kari->senderKey,
                                   secret, &secretSz);

    } else {
        /* bad direction */
        XFREE(secret, kari->heap, DYNAMIC_TYPE_PKCS7);
        return BAD_FUNC_ARG;
    }

    if (ret != 0) {
        XFREE(secret, kari->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    /* run through KDF */
    switch (keyEncOID) {

    #ifndef NO_SHA
        case dhSinglePass_stdDH_sha1kdf_scheme:
            kdfType = WC_HASH_TYPE_SHA;
            break;
    #endif
    #ifndef WOLFSSL_SHA224
        case dhSinglePass_stdDH_sha224kdf_scheme:
            kdfType = WC_HASH_TYPE_SHA224;
            break;
    #endif
    #ifndef NO_SHA256
        case dhSinglePass_stdDH_sha256kdf_scheme:
            kdfType = WC_HASH_TYPE_SHA256;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case dhSinglePass_stdDH_sha384kdf_scheme:
            kdfType = WC_HASH_TYPE_SHA384;
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case dhSinglePass_stdDH_sha512kdf_scheme:
            kdfType = WC_HASH_TYPE_SHA512;
            break;
    #endif
        default:
            WOLFSSL_MSG("Unsupported key agreement algorithm");
            XFREE(secret, kari->heap, DYNAMIC_TYPE_PKCS7);
            return BAD_FUNC_ARG;
    };

    ret = wc_X963_KDF(kdfType, secret, secretSz, kari->sharedInfo,
                      kari->sharedInfoSz, kari->kek, kari->kekSz);
    if (ret != 0) {
        XFREE(secret, kari->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    XFREE(secret, kari->heap, DYNAMIC_TYPE_PKCS7);

    return 0;
}


/* create ASN.1 formatted KeyAgreeRecipientInfo (kari) for use with ECDH,
 * return sequence size or negative on error */
static int wc_CreateKeyAgreeRecipientInfo(PKCS7* pkcs7, const byte* cert,
                            word32 certSz, int keyAgreeAlgo, int blockKeySz,
                            int keyWrapAlgo, int keyEncAlgo, WC_RNG* rng,
                            byte* contentKeyPlain, byte* contentKeyEnc,
                            int* keyEncSz, byte* out, word32 outSz)
{
    int ret = 0, idx = 0;
    int keySz, direction = 0;

    /* ASN.1 layout */
    int totalSz = 0;
    int kariSeqSz = 0;
    byte kariSeq[MAX_SEQ_SZ];           /* IMPLICIT [1] */
    int verSz = 0;
    byte ver[MAX_VERSION_SZ];

    int origIdOrKeySeqSz = 0;
    byte origIdOrKeySeq[MAX_SEQ_SZ];    /* IMPLICIT [0] */
    int origPubKeySeqSz = 0;
    byte origPubKeySeq[MAX_SEQ_SZ];     /* IMPLICIT [1] */
    int origAlgIdSz = 0;
    byte origAlgId[MAX_ALGO_SZ];
    int origPubKeyStrSz = 0;
    byte origPubKeyStr[MAX_OCTET_STR_SZ];

    /* optional user keying material */
    int ukmOctetSz = 0;
    byte ukmOctetStr[MAX_OCTET_STR_SZ];
    int ukmExplicitSz = 0;
    byte ukmExplicitSeq[MAX_SEQ_SZ];

    int keyEncryptAlgoIdSz = 0;
    byte keyEncryptAlgoId[MAX_ALGO_SZ];
    int keyWrapAlgSz = 0;
    byte keyWrapAlg[MAX_ALGO_SZ];

    int recipEncKeysSeqSz = 0;
    byte recipEncKeysSeq[MAX_SEQ_SZ];
    int recipEncKeySeqSz = 0;
    byte recipEncKeySeq[MAX_SEQ_SZ];
    int recipKeyIdSeqSz = 0;
    byte recipKeyIdSeq[MAX_SEQ_SZ];     /* IMPLICIT [0] */
    int subjKeyIdOctetSz = 0;
    byte subjKeyIdOctet[MAX_OCTET_STR_SZ];
    int encryptedKeyOctetSz = 0;
    byte encryptedKeyOctet[MAX_OCTET_STR_SZ];

    WC_PKCS7_KARI* kari;

    /* only supports ECDSA for now */
    if (keyAgreeAlgo != ECDSAk)
        return BAD_FUNC_ARG;

    /* set direction based on keyWrapAlgo */
    switch (keyWrapAlgo) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128_WRAP:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192_WRAP:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256_WRAP:
    #endif
            direction = AES_ENCRYPTION;
            break;
#endif
        default:
            WOLFSSL_MSG("Unsupported key wrap algorithm");
            return BAD_KEYWRAP_ALG_E;
    }

    kari = wc_PKCS7_KariNew(pkcs7, WC_PKCS7_ENCODE);
    if (kari == NULL)
        return MEMORY_E;

    /* set user keying material if available */
    if ((pkcs7->ukmSz > 0) && (pkcs7->ukm != NULL)) {
        kari->ukm = pkcs7->ukm;
        kari->ukmSz = pkcs7->ukmSz;
        kari->ukmOwner = 0;
    }

    /* parse recipient cert, get public key */
    ret = wc_PKCS7_KariParseRecipCert(kari, cert, certSz, NULL, 0);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        return ret;
    }

    /* generate sender ephemeral ECC key */
    ret = wc_PKCS7_KariGenerateEphemeralKey(kari, rng);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        return ret;
    }

    /* generate KEK (key encryption key) */
    ret = wc_PKCS7_KariGenerateKEK(kari, keyWrapAlgo, keyEncAlgo);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        return ret;
    }

    /* encrypt CEK with KEK */
    keySz = wc_PKCS7_KariKeyWrap(contentKeyPlain, blockKeySz, kari->kek,
                        kari->kekSz, contentKeyEnc, *keyEncSz, keyWrapAlgo,
                        direction);
    if (keySz <= 0) {
        wc_PKCS7_KariFree(kari);
        return ret;
    }
    *keyEncSz = (word32)keySz;

    /* Start of RecipientEncryptedKeys */

    /* EncryptedKey */
    encryptedKeyOctetSz = SetOctetString(*keyEncSz, encryptedKeyOctet);
    totalSz += (encryptedKeyOctetSz + *keyEncSz);

    /* SubjectKeyIdentifier */
    subjKeyIdOctetSz = SetOctetString(KEYID_SIZE, subjKeyIdOctet);
    totalSz += (subjKeyIdOctetSz + KEYID_SIZE);

    /* RecipientKeyIdentifier IMPLICIT [0] */
    recipKeyIdSeqSz = SetImplicit(ASN_SEQUENCE, 0, subjKeyIdOctetSz +
                                  KEYID_SIZE, recipKeyIdSeq);
    totalSz += recipKeyIdSeqSz;

    /* RecipientEncryptedKey */
    recipEncKeySeqSz = SetSequence(totalSz, recipEncKeySeq);
    totalSz += recipEncKeySeqSz;

    /* RecipientEncryptedKeys */
    recipEncKeysSeqSz = SetSequence(totalSz, recipEncKeysSeq);
    totalSz += recipEncKeysSeqSz;

    /* Start of optional UserKeyingMaterial */

    if (kari->ukmSz > 0) {
        ukmOctetSz = SetOctetString(kari->ukmSz, ukmOctetStr);
        totalSz += (ukmOctetSz + kari->ukmSz);

        ukmExplicitSz = SetExplicit(1, ukmOctetSz + kari->ukmSz,
                                    ukmExplicitSeq);
        totalSz += ukmExplicitSz;
    }

    /* Start of KeyEncryptionAlgorithmIdentifier */

    /* KeyWrapAlgorithm */
    keyWrapAlgSz = SetAlgoID(keyWrapAlgo, keyWrapAlg, oidKeyWrapType, 0);
    totalSz += keyWrapAlgSz;

    /* KeyEncryptionAlgorithmIdentifier */
    keyEncryptAlgoIdSz = SetAlgoID(keyEncAlgo, keyEncryptAlgoId,
                                   oidCmsKeyAgreeType, keyWrapAlgSz);
    totalSz += keyEncryptAlgoIdSz;

    /* Start of OriginatorIdentifierOrKey */

    /* recipient ECPoint, public key */
    XMEMSET(origPubKeyStr, 0, sizeof(origPubKeyStr)); /* no unused bits */
    origPubKeyStr[0] = ASN_BIT_STRING;
    origPubKeyStrSz = SetLength(kari->senderKeyExportSz + 1,
                                origPubKeyStr + 1) + 2;
    totalSz += (origPubKeyStrSz + kari->senderKeyExportSz);

    /* Originator AlgorithmIdentifier */
    origAlgIdSz = SetAlgoID(ECDSAk, origAlgId, oidKeyType, 0);
    totalSz += origAlgIdSz;

    /* outer OriginatorPublicKey IMPLICIT [1] */
    origPubKeySeqSz = SetImplicit(ASN_SEQUENCE, 1,
                                  origAlgIdSz + origPubKeyStrSz +
                                  kari->senderKeyExportSz, origPubKeySeq);
    totalSz += origPubKeySeqSz;

    /* outer OriginatorIdentiferOrKey IMPLICIT [0] */
    origIdOrKeySeqSz = SetImplicit(ASN_SEQUENCE, 0,
                                   origPubKeySeqSz + origAlgIdSz +
                                   origPubKeyStrSz + kari->senderKeyExportSz,
                                   origIdOrKeySeq);
    totalSz += origIdOrKeySeqSz;

    /* version, always 3 */
    verSz = SetMyVersion(3, ver, 0);
    totalSz += verSz;

    /* outer IMPLICIT [1] kari */
    kariSeqSz = SetImplicit(ASN_SEQUENCE, 1, totalSz, kariSeq);
    totalSz += kariSeqSz;

    if ((word32)totalSz > outSz) {
        WOLFSSL_MSG("KeyAgreeRecipientInfo output buffer too small");
        wc_PKCS7_KariFree(kari);

        return BUFFER_E;
    }

    XMEMCPY(out + idx, kariSeq, kariSeqSz);
    idx += kariSeqSz;
    XMEMCPY(out + idx, ver, verSz);
    idx += verSz;

    XMEMCPY(out + idx, origIdOrKeySeq, origIdOrKeySeqSz);
    idx += origIdOrKeySeqSz;
    XMEMCPY(out + idx, origPubKeySeq, origPubKeySeqSz);
    idx += origPubKeySeqSz;
    XMEMCPY(out + idx, origAlgId, origAlgIdSz);
    idx += origAlgIdSz;
    XMEMCPY(out + idx, origPubKeyStr, origPubKeyStrSz);
    idx += origPubKeyStrSz;
    /* ephemeral public key */
    XMEMCPY(out + idx, kari->senderKeyExport, kari->senderKeyExportSz);
    idx += kari->senderKeyExportSz;

    if (kari->ukmSz > 0) {
        XMEMCPY(out + idx, ukmExplicitSeq, ukmExplicitSz);
        idx += ukmExplicitSz;
        XMEMCPY(out + idx, ukmOctetStr, ukmOctetSz);
        idx += ukmOctetSz;
        XMEMCPY(out + idx, kari->ukm, kari->ukmSz);
        idx += kari->ukmSz;
    }

    XMEMCPY(out + idx, keyEncryptAlgoId, keyEncryptAlgoIdSz);
    idx += keyEncryptAlgoIdSz;
    XMEMCPY(out + idx, keyWrapAlg, keyWrapAlgSz);
    idx += keyWrapAlgSz;

    XMEMCPY(out + idx, recipEncKeysSeq, recipEncKeysSeqSz);
    idx += recipEncKeysSeqSz;
    XMEMCPY(out + idx, recipEncKeySeq, recipEncKeySeqSz);
    idx += recipEncKeySeqSz;
    XMEMCPY(out + idx, recipKeyIdSeq, recipKeyIdSeqSz);
    idx += recipKeyIdSeqSz;
    XMEMCPY(out + idx, subjKeyIdOctet, subjKeyIdOctetSz);
    idx += subjKeyIdOctetSz;
    /* subject key id */
    XMEMCPY(out + idx, kari->decoded->extSubjKeyId, KEYID_SIZE);
    idx += KEYID_SIZE;
    XMEMCPY(out + idx, encryptedKeyOctet, encryptedKeyOctetSz);
    idx += encryptedKeyOctetSz;
    /* encrypted CEK */
    XMEMCPY(out + idx, contentKeyEnc, *keyEncSz);
    idx += *keyEncSz;

    wc_PKCS7_KariFree(kari);

    return idx;
}

#endif /* HAVE_ECC */

#ifndef NO_RSA

/* create ASN.1 formatted RecipientInfo structure, returns sequence size */
static int wc_CreateRecipientInfo(const byte* cert, word32 certSz,
                                  int keyEncAlgo, int blockKeySz,
                                  WC_RNG* rng, byte* contentKeyPlain,
                                  byte* contentKeyEnc, int* keyEncSz,
                                  byte* out, word32 outSz, void* heap)
{
    word32 idx = 0;
    int ret = 0, totalSz = 0;
    int verSz, issuerSz, snSz, keyEncAlgSz;
    int issuerSeqSz, recipSeqSz, issuerSerialSeqSz;
    int encKeyOctetStrSz;

    byte ver[MAX_VERSION_SZ];
    byte issuerSerialSeq[MAX_SEQ_SZ];
    byte recipSeq[MAX_SEQ_SZ];
    byte issuerSeq[MAX_SEQ_SZ];
    byte encKeyOctetStr[MAX_OCTET_STR_SZ];

#ifdef WOLFSSL_SMALL_STACK
    byte *serial;
    byte *keyAlgArray;

    RsaKey* pubKey;
    DecodedCert* decoded;

    serial = (byte*)XMALLOC(MAX_SN_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    keyAlgArray = (byte*)XMALLOC(MAX_SN_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);

    if (decoded == NULL || serial == NULL || keyAlgArray == NULL) {
        if (serial)      XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (keyAlgArray) XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (decoded)     XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }

#else
    byte serial[MAX_SN_SZ];
    byte keyAlgArray[MAX_ALGO_SZ];

    RsaKey stack_pubKey;
    RsaKey* pubKey = &stack_pubKey;
    DecodedCert stack_decoded;
    DecodedCert* decoded = &stack_decoded;
#endif

    InitDecodedCert(decoded, (byte*)cert, certSz, heap);
    ret = ParseCert(decoded, CA_TYPE, NO_VERIFY, 0);
    if (ret < 0) {
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* version */
    verSz = SetMyVersion(0, ver, 0);

    /* IssuerAndSerialNumber */
    if (decoded->issuerRaw == NULL || decoded->issuerRawLen == 0) {
        WOLFSSL_MSG("DecodedCert lacks raw issuer pointer and length");
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return -1;
    }
    issuerSz    = decoded->issuerRawLen;
    issuerSeqSz = SetSequence(issuerSz, issuerSeq);

    if (decoded->serialSz == 0) {
        WOLFSSL_MSG("DecodedCert missing serial number");
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return -1;
    }
    snSz = SetSerialNumber(decoded->serial, decoded->serialSz, serial, MAX_SN_SZ);

    issuerSerialSeqSz = SetSequence(issuerSeqSz + issuerSz + snSz,
                                    issuerSerialSeq);

    /* KeyEncryptionAlgorithmIdentifier, only support RSA now */
    if (keyEncAlgo != RSAk) {
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ALGO_ID_E;
    }

    keyEncAlgSz = SetAlgoID(keyEncAlgo, keyAlgArray, oidKeyType, 0);
    if (keyEncAlgSz == 0) {
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_SMALL_STACK
    pubKey = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pubKey == NULL) {
        FreeDecodedCert(decoded);
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    /* EncryptedKey */
    ret = wc_InitRsaKey_ex(pubKey, heap, INVALID_DEVID);
    if (ret != 0) {
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(pubKey,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (wc_RsaPublicKeyDecode(decoded->publicKey, &idx, pubKey,
                           decoded->pubKeySize) < 0) {
        WOLFSSL_MSG("ASN RSA key decode error");
        wc_FreeRsaKey(pubKey);
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(pubKey,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return PUBLIC_KEY_E;
    }

    *keyEncSz = wc_RsaPublicEncrypt(contentKeyPlain, blockKeySz, contentKeyEnc,
                                 MAX_ENCRYPTED_KEY_SZ, pubKey, rng);
    wc_FreeRsaKey(pubKey);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(pubKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (*keyEncSz < 0) {
        WOLFSSL_MSG("RSA Public Encrypt failed");
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return *keyEncSz;
    }

    encKeyOctetStrSz = SetOctetString(*keyEncSz, encKeyOctetStr);

    /* RecipientInfo */
    recipSeqSz = SetSequence(verSz + issuerSerialSeqSz + issuerSeqSz +
                             issuerSz + snSz + keyEncAlgSz + encKeyOctetStrSz +
                             *keyEncSz, recipSeq);

    if (recipSeqSz + verSz + issuerSerialSeqSz + issuerSeqSz + snSz +
        keyEncAlgSz + encKeyOctetStrSz + *keyEncSz > (int)outSz) {
        WOLFSSL_MSG("RecipientInfo output buffer too small");
        FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BUFFER_E;
    }

    XMEMCPY(out + totalSz, recipSeq, recipSeqSz);
    totalSz += recipSeqSz;
    XMEMCPY(out + totalSz, ver, verSz);
    totalSz += verSz;
    XMEMCPY(out + totalSz, issuerSerialSeq, issuerSerialSeqSz);
    totalSz += issuerSerialSeqSz;
    XMEMCPY(out + totalSz, issuerSeq, issuerSeqSz);
    totalSz += issuerSeqSz;
    XMEMCPY(out + totalSz, decoded->issuerRaw, issuerSz);
    totalSz += issuerSz;
    XMEMCPY(out + totalSz, serial, snSz);
    totalSz += snSz;
    XMEMCPY(out + totalSz, keyAlgArray, keyEncAlgSz);
    totalSz += keyEncAlgSz;
    XMEMCPY(out + totalSz, encKeyOctetStr, encKeyOctetStrSz);
    totalSz += encKeyOctetStrSz;
    XMEMCPY(out + totalSz, contentKeyEnc, *keyEncSz);
    totalSz += *keyEncSz;

    FreeDecodedCert(decoded);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(keyAlgArray, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(decoded,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return totalSz;
}
#endif /* !NO_RSA */


/* encrypt content using encryptOID algo */
static int wc_PKCS7_EncryptContent(int encryptOID, byte* key, int keySz,
                                   byte* iv, int ivSz, byte* in, int inSz,
                                   byte* out)
{
    int ret;
#ifndef NO_AES
    Aes  aes;
#endif
#ifndef NO_DES3
    Des  des;
    Des3 des3;
#endif

    if (key == NULL || iv == NULL || in == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (encryptOID) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128CBCb:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192CBCb:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256CBCb:
    #endif
            if (
                #ifdef WOLFSSL_AES_128
                    (encryptOID == AES128CBCb && keySz != 16 ) ||
                #endif
                #ifdef WOLFSSL_AES_192
                    (encryptOID == AES192CBCb && keySz != 24 ) ||
                #endif
                #ifdef WOLFSSL_AES_256
                    (encryptOID == AES256CBCb && keySz != 32 ) ||
                #endif
                    (ivSz  != AES_BLOCK_SIZE) )
                return BAD_FUNC_ARG;

            ret = wc_AesSetKey(&aes, key, keySz, iv, AES_ENCRYPTION);
            if (ret == 0)
                ret = wc_AesCbcEncrypt(&aes, out, in, inSz);

            break;
#endif
#ifndef NO_DES3
        case DESb:
            if (keySz != DES_KEYLEN || ivSz != DES_BLOCK_SIZE)
                return BAD_FUNC_ARG;

            ret = wc_Des_SetKey(&des, key, iv, DES_ENCRYPTION);
            if (ret == 0)
                ret = wc_Des_CbcEncrypt(&des, out, in, inSz);

            break;

        case DES3b:
            if (keySz != DES3_KEYLEN || ivSz != DES_BLOCK_SIZE)
                return BAD_FUNC_ARG;

            ret = wc_Des3_SetKey(&des3, key, iv, DES_ENCRYPTION);
            if (ret == 0)
                ret = wc_Des3_CbcEncrypt(&des3, out, in, inSz);

            break;
#endif
        default:
            WOLFSSL_MSG("Unsupported content cipher type");
            return ALGO_ID_E;
    };

    return ret;
}


/* decrypt content using encryptOID algo */
static int wc_PKCS7_DecryptContent(int encryptOID, byte* key, int keySz,
                                   byte* iv, int ivSz, byte* in, int inSz,
                                   byte* out)
{
    int ret;
#ifndef NO_AES
    Aes  aes;
#endif
#ifndef NO_DES3
    Des  des;
    Des3 des3;
#endif

    if (key == NULL || iv == NULL || in == NULL || out == NULL)
        return BAD_FUNC_ARG;

    switch (encryptOID) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128CBCb:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192CBCb:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256CBCb:
    #endif
            if (
                #ifdef WOLFSSL_AES_128
                    (encryptOID == AES128CBCb && keySz != 16 ) ||
                #endif
                #ifdef WOLFSSL_AES_192
                    (encryptOID == AES192CBCb && keySz != 24 ) ||
                #endif
                #ifdef WOLFSSL_AES_256
                    (encryptOID == AES256CBCb && keySz != 32 ) ||
                #endif
                    (ivSz  != AES_BLOCK_SIZE) )
                return BAD_FUNC_ARG;

            ret = wc_AesSetKey(&aes, key, keySz, iv, AES_DECRYPTION);
            if (ret == 0)
                ret = wc_AesCbcDecrypt(&aes, out, in, inSz);

            break;
#endif
#ifndef NO_DES3
        case DESb:
            if (keySz != DES_KEYLEN || ivSz != DES_BLOCK_SIZE)
                return BAD_FUNC_ARG;

            ret = wc_Des_SetKey(&des, key, iv, DES_DECRYPTION);
            if (ret == 0)
                ret = wc_Des_CbcDecrypt(&des, out, in, inSz);

            break;
        case DES3b:
            if (keySz != DES3_KEYLEN || ivSz != DES_BLOCK_SIZE)
                return BAD_FUNC_ARG;

            ret = wc_Des3_SetKey(&des3, key, iv, DES_DECRYPTION);
            if (ret == 0)
                ret = wc_Des3_CbcDecrypt(&des3, out, in, inSz);

            break;
#endif
        default:
            WOLFSSL_MSG("Unsupported content cipher type");
            return ALGO_ID_E;
    };

    return ret;
}


/* generate random IV, place in iv, return 0 on success negative on error */
static int wc_PKCS7_GenerateIV(PKCS7* pkcs7, WC_RNG* rng, byte* iv, word32 ivSz)
{
    int ret;
    WC_RNG* rnd = NULL;

    if (iv == NULL || ivSz == 0)
        return BAD_FUNC_ARG;

    /* input RNG is optional, init local one if input rng is NULL */
    if (rng == NULL) {
        rnd = (WC_RNG*)XMALLOC(sizeof(WC_RNG), pkcs7->heap, DYNAMIC_TYPE_RNG);
        if (rnd == NULL)
            return MEMORY_E;

        ret = wc_InitRng_ex(rnd, pkcs7->heap, pkcs7->devId);
        if (ret != 0) {
            XFREE(rnd, pkcs7->heap, DYNAMIC_TYPE_RNG);
            return ret;
        }

    } else {
        rnd = rng;
    }

    ret = wc_RNG_GenerateBlock(rnd, iv, ivSz);

    if (rng == NULL) {
        wc_FreeRng(rnd);
        XFREE(rnd, pkcs7->heap, DYNAMIC_TYPE_RNG);
    }

    return ret;
}


/* return size of padded data, padded to blockSz chunks, or negative on error */
int wc_PKCS7_GetPadSize(word32 inputSz, word32 blockSz)
{
    int padSz;

    if (blockSz == 0)
        return BAD_FUNC_ARG;

    padSz = blockSz - (inputSz % blockSz);

    return padSz;
}


/* pad input data to blockSz chunk, place in outSz. out must be big enough
 * for input + pad bytes. See wc_PKCS7_GetPadSize() helper. */
int wc_PKCS7_PadData(byte* in, word32 inSz, byte* out, word32 outSz,
                     word32 blockSz)
{
    int i, padSz;

    if (in == NULL  || inSz == 0 ||
        out == NULL || outSz == 0)
        return BAD_FUNC_ARG;

    padSz = wc_PKCS7_GetPadSize(inSz, blockSz);

    if (outSz < (inSz + padSz))
        return BAD_FUNC_ARG;

    XMEMCPY(out, in, inSz);

    for (i = 0; i < padSz; i++) {
        out[inSz + i] = (byte)padSz;
    }

    return inSz + padSz;
}


/* build PKCS#7 envelopedData content type, return enveloped size */
int wc_PKCS7_EncodeEnvelopedData(PKCS7* pkcs7, byte* output, word32 outputSz)
{
    int ret, idx = 0;
    int totalSz, padSz, encryptedOutSz;

    int contentInfoSeqSz, outerContentTypeSz, outerContentSz;
    byte contentInfoSeq[MAX_SEQ_SZ];
    byte outerContentType[MAX_ALGO_SZ];
    byte outerContent[MAX_SEQ_SZ];

    int envDataSeqSz, verSz;
    byte envDataSeq[MAX_SEQ_SZ];
    byte ver[MAX_VERSION_SZ];

    WC_RNG rng;
    int contentKeyEncSz, blockSz, blockKeySz;
    byte contentKeyPlain[MAX_CONTENT_KEY_LEN];
#ifdef WOLFSSL_SMALL_STACK
    byte* contentKeyEnc;
#else
    byte contentKeyEnc[MAX_ENCRYPTED_KEY_SZ];
#endif
    byte* plain;
    byte* encryptedContent;

    int recipSz, recipSetSz;
#ifdef WOLFSSL_SMALL_STACK
    byte* recip;
#else
    byte recip[MAX_RECIP_SZ];
#endif
    byte recipSet[MAX_SET_SZ];

    int encContentOctetSz, encContentSeqSz, contentTypeSz;
    int contentEncAlgoSz, ivOctetStringSz;
    byte encContentSeq[MAX_SEQ_SZ];
    byte contentType[MAX_ALGO_SZ];
    byte contentEncAlgo[MAX_ALGO_SZ];
    byte tmpIv[MAX_CONTENT_IV_SIZE];
    byte ivOctetString[MAX_OCTET_STR_SZ];
    byte encContentOctet[MAX_OCTET_STR_SZ];

    if (pkcs7 == NULL || pkcs7->content == NULL || pkcs7->contentSz == 0 ||
        pkcs7->encryptOID == 0 || pkcs7->singleCert == NULL ||
        pkcs7->publicKeyOID == 0)
        return BAD_FUNC_ARG;

    if (output == NULL || outputSz == 0)
        return BAD_FUNC_ARG;

    blockKeySz = wc_PKCS7_GetOIDKeySize(pkcs7->encryptOID);
    if (blockKeySz < 0)
        return blockKeySz;

    blockSz = wc_PKCS7_GetOIDBlockSize(pkcs7->encryptOID);
    if (blockSz < 0)
        return blockSz;

    /* outer content type */
    outerContentTypeSz = wc_SetContentType(ENVELOPED_DATA, outerContentType);

    /* version, defined as 0 in RFC 2315 */
#ifdef HAVE_ECC
    if (pkcs7->publicKeyOID == ECDSAk) {
        verSz = SetMyVersion(2, ver, 0);
    } else
#endif
    {
        verSz = SetMyVersion(0, ver, 0);
    }

    /* generate random content encryption key */
    ret = wc_InitRng_ex(&rng, pkcs7->heap, pkcs7->devId);
    if (ret != 0)
        return ret;

    ret = wc_RNG_GenerateBlock(&rng, contentKeyPlain, blockKeySz);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }

#ifdef WOLFSSL_SMALL_STACK
    recip         = (byte*)XMALLOC(MAX_RECIP_SZ, NULL, DYNAMIC_TYPE_PKCS7);
    contentKeyEnc = (byte*)XMALLOC(MAX_ENCRYPTED_KEY_SZ, NULL,
                                                       DYNAMIC_TYPE_PKCS7);
    if (contentKeyEnc == NULL || recip == NULL) {
        if (recip)         XFREE(recip,         NULL, DYNAMIC_TYPE_PKCS7);
        if (contentKeyEnc) XFREE(contentKeyEnc, NULL, DYNAMIC_TYPE_PKCS7);
        wc_FreeRng(&rng);
        return MEMORY_E;
    }
#endif
    contentKeyEncSz = MAX_ENCRYPTED_KEY_SZ;

    /* build RecipientInfo, only handle 1 for now */
    switch (pkcs7->publicKeyOID) {
#ifndef NO_RSA
        case RSAk:
            recipSz = wc_CreateRecipientInfo(pkcs7->singleCert,
                                    pkcs7->singleCertSz,
                                    pkcs7->publicKeyOID,
                                    blockKeySz, &rng, contentKeyPlain,
                                    contentKeyEnc, &contentKeyEncSz, recip,
                                    MAX_RECIP_SZ, pkcs7->heap);
            break;
#endif
#ifdef HAVE_ECC
        case ECDSAk:
            recipSz = wc_CreateKeyAgreeRecipientInfo(pkcs7, pkcs7->singleCert,
                                    pkcs7->singleCertSz,
                                    pkcs7->publicKeyOID,
                                    blockKeySz, pkcs7->keyWrapOID,
                                    pkcs7->keyAgreeOID, &rng,
                                    contentKeyPlain, contentKeyEnc,
                                    &contentKeyEncSz, recip, MAX_RECIP_SZ);
            break;
#endif

        default:
            WOLFSSL_MSG("Unsupported RecipientInfo public key type");
            return BAD_FUNC_ARG;
    };

    ForceZero(contentKeyEnc, MAX_ENCRYPTED_KEY_SZ);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(contentKeyEnc, NULL, DYNAMIC_TYPE_PKCS7);
#endif

    if (recipSz < 0) {
        WOLFSSL_MSG("Failed to create RecipientInfo");
        wc_FreeRng(&rng);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return recipSz;
    }
    recipSetSz = SetSet(recipSz, recipSet);

    /* generate IV for block cipher */
    ret = wc_PKCS7_GenerateIV(pkcs7, &rng, tmpIv, blockSz);
    wc_FreeRng(&rng);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* EncryptedContentInfo */
    contentTypeSz = wc_SetContentType(pkcs7->contentOID, contentType);
    if (contentTypeSz == 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }

    /* allocate encrypted content buffer and PKCS#7 padding */
    padSz = wc_PKCS7_GetPadSize(pkcs7->contentSz, blockSz);
    if (padSz < 0)
        return padSz;

    encryptedOutSz = pkcs7->contentSz + padSz;

    plain = (byte*)XMALLOC(encryptedOutSz, pkcs7->heap,
                           DYNAMIC_TYPE_PKCS7);
    if (plain == NULL)
        return MEMORY_E;

    ret = wc_PKCS7_PadData(pkcs7->content, pkcs7->contentSz, plain,
                           encryptedOutSz, blockSz);
    if (ret < 0) {
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    encryptedContent = (byte*)XMALLOC(encryptedOutSz, pkcs7->heap,
                                      DYNAMIC_TYPE_PKCS7);
    if (encryptedContent == NULL) {
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return MEMORY_E;
    }

    /* put together IV OCTET STRING */
    ivOctetStringSz = SetOctetString(blockSz, ivOctetString);

    /* build up our ContentEncryptionAlgorithmIdentifier sequence,
     * adding (ivOctetStringSz + blockSz) for IV OCTET STRING */
    contentEncAlgoSz = SetAlgoID(pkcs7->encryptOID, contentEncAlgo,
                                 oidBlkType, ivOctetStringSz + blockSz);

    if (contentEncAlgoSz == 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return BAD_FUNC_ARG;
    }

    /* encrypt content */
    ret = wc_PKCS7_EncryptContent(pkcs7->encryptOID, contentKeyPlain,
            blockKeySz, tmpIv, blockSz, plain, encryptedOutSz,
            encryptedContent);

    if (ret != 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ret;
    }

    encContentOctetSz = SetImplicit(ASN_OCTET_STRING, 0, encryptedOutSz,
                                    encContentOctet);

    encContentSeqSz = SetSequence(contentTypeSz + contentEncAlgoSz +
                                  ivOctetStringSz + blockSz +
                                  encContentOctetSz + encryptedOutSz,
                                  encContentSeq);

    /* keep track of sizes for outer wrapper layering */
    totalSz = verSz + recipSetSz + recipSz + encContentSeqSz + contentTypeSz +
              contentEncAlgoSz + ivOctetStringSz + blockSz +
              encContentOctetSz + encryptedOutSz;

    /* EnvelopedData */
    envDataSeqSz = SetSequence(totalSz, envDataSeq);
    totalSz += envDataSeqSz;

    /* outer content */
    outerContentSz = SetExplicit(0, totalSz, outerContent);
    totalSz += outerContentTypeSz;
    totalSz += outerContentSz;

    /* ContentInfo */
    contentInfoSeqSz = SetSequence(totalSz, contentInfoSeq);
    totalSz += contentInfoSeqSz;

    if (totalSz > (int)outputSz) {
        WOLFSSL_MSG("Pkcs7_encrypt output buffer too small");
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_TMP_BUFFER);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BUFFER_E;
    }

    XMEMCPY(output + idx, contentInfoSeq, contentInfoSeqSz);
    idx += contentInfoSeqSz;
    XMEMCPY(output + idx, outerContentType, outerContentTypeSz);
    idx += outerContentTypeSz;
    XMEMCPY(output + idx, outerContent, outerContentSz);
    idx += outerContentSz;
    XMEMCPY(output + idx, envDataSeq, envDataSeqSz);
    idx += envDataSeqSz;
    XMEMCPY(output + idx, ver, verSz);
    idx += verSz;
    XMEMCPY(output + idx, recipSet, recipSetSz);
    idx += recipSetSz;
    XMEMCPY(output + idx, recip, recipSz);
    idx += recipSz;
    XMEMCPY(output + idx, encContentSeq, encContentSeqSz);
    idx += encContentSeqSz;
    XMEMCPY(output + idx, contentType, contentTypeSz);
    idx += contentTypeSz;
    XMEMCPY(output + idx, contentEncAlgo, contentEncAlgoSz);
    idx += contentEncAlgoSz;
    XMEMCPY(output + idx, ivOctetString, ivOctetStringSz);
    idx += ivOctetStringSz;
    XMEMCPY(output + idx, tmpIv, blockSz);
    idx += blockSz;
    XMEMCPY(output + idx, encContentOctet, encContentOctetSz);
    idx += encContentOctetSz;
    XMEMCPY(output + idx, encryptedContent, encryptedOutSz);
    idx += encryptedOutSz;

    ForceZero(contentKeyPlain, MAX_CONTENT_KEY_LEN);

    XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
    XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(recip, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return idx;
}

#ifndef NO_RSA
/* decode KeyTransRecipientInfo (ktri), return 0 on success, <0 on error */
static int wc_PKCS7_DecodeKtri(PKCS7* pkcs7, byte* pkiMsg, word32 pkiMsgSz,
                               word32* idx, byte* decryptedKey,
                               word32* decryptedKeySz, int* recipFound)
{
    int length, encryptedKeySz, ret;
    int keySz;
    word32 encOID;
    word32 keyIdx;
    byte   issuerHash[KEYID_SIZE];
    byte*  outKey = NULL;

#ifdef WC_RSA_BLINDING
    WC_RNG rng;
#endif

#ifdef WOLFSSL_SMALL_STACK
    mp_int* serialNum;
    byte* encryptedKey;
    RsaKey* privKey;
#else
    mp_int stack_serialNum;
    mp_int* serialNum = &stack_serialNum;
    byte encryptedKey[MAX_ENCRYPTED_KEY_SZ];

    RsaKey stack_privKey;
    RsaKey* privKey = &stack_privKey;
#endif

    /* remove IssuerAndSerialNumber */
    if (GetSequence(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (GetNameHash(pkiMsg, idx, issuerHash, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* if we found correct recipient, issuer hashes will match */
    if (XMEMCMP(issuerHash, pkcs7->issuerHash, KEYID_SIZE) == 0) {
        *recipFound = 1;
    }

#ifdef WOLFSSL_SMALL_STACK
    serialNum = (mp_int*)XMALLOC(sizeof(mp_int), NULL,
                                                   DYNAMIC_TYPE_TMP_BUFFER);
    if (serialNum == NULL)
        return MEMORY_E;
#endif

    if (GetInt(serialNum, pkiMsg, idx, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serialNum,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ASN_PARSE_E;
    }

    mp_clear(serialNum);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(serialNum, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (GetAlgoId(pkiMsg, idx, &encOID, oidKeyType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* key encryption algorithm must be RSA for now */
    if (encOID != RSAk)
        return ALGO_ID_E;

    /* read encryptedKey */
#ifdef WOLFSSL_SMALL_STACK
    encryptedKey = (byte*)XMALLOC(MAX_ENCRYPTED_KEY_SZ, NULL,
                                  DYNAMIC_TYPE_TMP_BUFFER);
    if (encryptedKey == NULL)
        return MEMORY_E;
#endif

    if (pkiMsg[(*idx)++] != ASN_OCTET_STRING) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ASN_PARSE_E;
    }

    if (GetLength(pkiMsg, idx, &encryptedKeySz, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ASN_PARSE_E;
    }

    if (*recipFound == 1)
        XMEMCPY(encryptedKey, &pkiMsg[*idx], encryptedKeySz);
    *idx += encryptedKeySz;

    /* load private key */
#ifdef WOLFSSL_SMALL_STACK
    privKey = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (privKey == NULL) {
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    ret = wc_InitRsaKey_ex(privKey, NULL, INVALID_DEVID);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (pkcs7->privateKey != NULL && pkcs7->privateKeySz > 0) {
        keyIdx = 0;
        ret = wc_RsaPrivateKeyDecode(pkcs7->privateKey, &keyIdx, privKey,
                                     pkcs7->privateKeySz);
    }
    else if (pkcs7->devId == INVALID_DEVID) {
        ret = BAD_FUNC_ARG;
    }
    if (ret != 0) {
        WOLFSSL_MSG("Failed to decode RSA private key");
        wc_FreeRsaKey(privKey);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* decrypt encryptedKey */
    #ifdef WC_RSA_BLINDING
    ret = wc_InitRng_ex(&rng, pkcs7->heap, pkcs7->devId);
    if (ret == 0) {
        ret = wc_RsaSetRNG(privKey, &rng);
    }
    #endif
    if (ret == 0) {
        keySz = wc_RsaPrivateDecryptInline(encryptedKey, encryptedKeySz,
                                           &outKey, privKey);
        #ifdef WC_RSA_BLINDING
            wc_FreeRng(&rng);
        #endif
    } else {
        keySz = ret;
    }
    wc_FreeRsaKey(privKey);

    if (keySz <= 0 || outKey == NULL) {
        ForceZero(encryptedKey, MAX_ENCRYPTED_KEY_SZ);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return keySz;
    } else {
        *decryptedKeySz = keySz;
        XMEMCPY(decryptedKey, outKey, keySz);
        ForceZero(encryptedKey, MAX_ENCRYPTED_KEY_SZ);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(encryptedKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(privKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}
#endif /* !NO_RSA */

#ifdef HAVE_ECC

/* remove ASN.1 OriginatorIdentifierOrKey, return 0 on success, <0 on error */
static int wc_PKCS7_KariGetOriginatorIdentifierOrKey(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx)
{
    int ret, length;
    word32 keyOID;

    if (kari == NULL || pkiMsg == NULL || idx == NULL)
        return BAD_FUNC_ARG;

    /* remove OriginatorIdentifierOrKey */
    if (pkiMsg[*idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)) {
        (*idx)++;
        if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

    } else {
        return ASN_PARSE_E;
    }

    /* remove OriginatorPublicKey */
    if (pkiMsg[*idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1)) {
        (*idx)++;
        if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

    } else {
        return ASN_PARSE_E;
    }

    /* remove AlgorithmIdentifier */
    if (GetAlgoId(pkiMsg, idx, &keyOID, oidKeyType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (keyOID != ECDSAk)
        return ASN_PARSE_E;

    /* remove ECPoint BIT STRING */
    if ((pkiMsgSz > (*idx + 1)) && (pkiMsg[(*idx)++] != ASN_BIT_STRING))
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if ((pkiMsgSz < (*idx + 1)) || (pkiMsg[(*idx)++] != 0x00))
        return ASN_EXPECT_0_E;

    /* get sender ephemeral public ECDSA key */
    ret = wc_ecc_init_ex(kari->senderKey, kari->heap, kari->devId);
    if (ret != 0)
        return ret;

    kari->senderKeyInit = 1;

    /* length-1 for unused bits counter */
    ret = wc_ecc_import_x963(pkiMsg + (*idx), length - 1, kari->senderKey);
    if (ret != 0)
        return ret;

    (*idx) += length - 1;

    return 0;
}


/* remove optional UserKeyingMaterial if available, return 0 on success,
 * < 0 on error */
static int wc_PKCS7_KariGetUserKeyingMaterial(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx)
{
    int length;
    word32 savedIdx;

    if (kari == NULL || pkiMsg == NULL || idx == NULL)
        return BAD_FUNC_ARG;

    savedIdx = *idx;

    /* starts with EXPLICIT [1] */
    if (pkiMsg[(*idx)++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1)) {
        *idx = savedIdx;
        return 0;
    }

    if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0) {
        *idx = savedIdx;
        return 0;
    }

    /* get OCTET STRING */
    if ( (pkiMsgSz > ((*idx) + 1)) &&
         (pkiMsg[(*idx)++] != ASN_OCTET_STRING) ) {
        *idx = savedIdx;
        return 0;
    }

    if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0) {
        *idx = savedIdx;
        return 0;
    }

    kari->ukm = NULL;
    if (length > 0) {
        kari->ukm = (byte*)XMALLOC(length, kari->heap, DYNAMIC_TYPE_PKCS7);
        if (kari->ukm == NULL)
            return MEMORY_E;

        XMEMCPY(kari->ukm, pkiMsg + (*idx), length);
        kari->ukmOwner = 1;
    }

    (*idx) += length;
    kari->ukmSz = length;

    return 0;
}


/* remove ASN.1 KeyEncryptionAlgorithmIdentifier, return 0 on success,
 * < 0 on error */
static int wc_PKCS7_KariGetKeyEncryptionAlgorithmId(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx,
                        word32* keyAgreeOID, word32* keyWrapOID)
{
    if (kari == NULL || pkiMsg == NULL || idx == NULL ||
        keyAgreeOID == NULL || keyWrapOID == NULL)
        return BAD_FUNC_ARG;

    /* remove KeyEncryptionAlgorithmIdentifier */
    if (GetAlgoId(pkiMsg, idx, keyAgreeOID, oidCmsKeyAgreeType,
                  pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* remove KeyWrapAlgorithm, stored in parameter of KeyEncAlgoId */
    if (GetAlgoId(pkiMsg, idx, keyWrapOID, oidKeyWrapType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    return 0;
}


/* remove ASN.1 SubjectKeyIdentifier, return 0 on success, < 0 on error
 * if subject key ID matches, recipFound is set to 1 */
static int wc_PKCS7_KariGetSubjectKeyIdentifier(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx,
                        int* recipFound)
{
    int length;
    byte subjKeyId[KEYID_SIZE];

    if (kari == NULL || pkiMsg == NULL || idx == NULL || recipFound == NULL)
        return BAD_FUNC_ARG;

    /* remove RecipientKeyIdentifier IMPLICIT [0] */
    if ( (pkiMsgSz > (*idx + 1)) &&
         (pkiMsg[(*idx)++] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)) ) {

        if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;

    } else {
        return ASN_PARSE_E;
    }

    /* remove SubjectKeyIdentifier */
    if ( (pkiMsgSz > (*idx + 1)) &&
         (pkiMsg[(*idx)++] != ASN_OCTET_STRING) )
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (length != KEYID_SIZE)
        return ASN_PARSE_E;

    XMEMCPY(subjKeyId, pkiMsg + (*idx), KEYID_SIZE);
    (*idx) += length;

    /* subject key id should match if recipient found */
    if (XMEMCMP(subjKeyId, kari->decoded->extSubjKeyId, KEYID_SIZE) == 0) {
        *recipFound = 1;
    }

    return 0;
}


/* remove ASN.1 IssuerAndSerialNumber, return 0 on success, < 0 on error
 * if issuer and serial number match, recipFound is set to 1 */
static int wc_PKCS7_KariGetIssuerAndSerialNumber(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx,
                        int* recipFound)
{
    int length, ret;
    byte issuerHash[KEYID_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    mp_int* serial;
    mp_int* recipSerial;
#else
    mp_int  stack_serial;
    mp_int* serial = &stack_serial;

    mp_int  stack_recipSerial;
    mp_int* recipSerial = &stack_recipSerial;
#endif

    /* remove IssuerAndSerialNumber */
    if (GetSequence(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (GetNameHash(pkiMsg, idx, issuerHash, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* if we found correct recipient, issuer hashes will match */
    if (XMEMCMP(issuerHash, kari->decoded->issuerHash, KEYID_SIZE) == 0) {
        *recipFound = 1;
    }

#ifdef WOLFSSL_SMALL_STACK
    serial = (mp_int*)XMALLOC(sizeof(mp_int), NULL,
                              DYNAMIC_TYPE_TMP_BUFFER);
    if (serial == NULL)
        return MEMORY_E;

    recipSerial = (mp_int*)XMALLOC(sizeof(mp_int), NULL,
                                   DYNAMIC_TYPE_TMP_BUFFER);
    if (recipSerial == NULL) {
        XFREE(serial, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    if (GetInt(serial, pkiMsg, idx, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(recipSerial, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ASN_PARSE_E;
    }

    ret = mp_read_unsigned_bin(recipSerial, kari->decoded->serial,
                             kari->decoded->serialSz);
    if (ret != MP_OKAY) {
        mp_clear(serial);
        WOLFSSL_MSG("Failed to parse CMS recipient serial number");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(recipSerial, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    if (mp_cmp(recipSerial, serial) != MP_EQ) {
        mp_clear(serial);
        mp_clear(recipSerial);
        WOLFSSL_MSG("CMS serial number does not match recipient");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(recipSerial, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return PKCS7_RECIP_E;
    }

    mp_clear(serial);
    mp_clear(recipSerial);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(serial,      NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(recipSerial, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}


/* remove ASN.1 RecipientEncryptedKeys, return 0 on success, < 0 on error */
static int wc_PKCS7_KariGetRecipientEncryptedKeys(WC_PKCS7_KARI* kari,
                        byte* pkiMsg, word32 pkiMsgSz, word32* idx,
                        int* recipFound, byte* encryptedKey,
                        int* encryptedKeySz)
{
    int length;
    int ret = 0;

    if (kari == NULL || pkiMsg == NULL || idx == NULL ||
        recipFound == NULL || encryptedKey == NULL)
        return BAD_FUNC_ARG;

    /* remove RecipientEncryptedKeys */
    if (GetSequence(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* remove RecipientEncryptedKeys */
    if (GetSequence(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* KeyAgreeRecipientIdentifier is CHOICE of IssuerAndSerialNumber
     * or [0] IMMPLICIT RecipientKeyIdentifier */
    if ( (pkiMsgSz > (*idx + 1)) &&
         (pkiMsg[*idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)) ) {

        /* try to get RecipientKeyIdentifier */
        ret = wc_PKCS7_KariGetSubjectKeyIdentifier(kari, pkiMsg, pkiMsgSz,
                                                   idx, recipFound);
    } else {
        /* try to get IssuerAndSerialNumber */
        ret = wc_PKCS7_KariGetIssuerAndSerialNumber(kari, pkiMsg, pkiMsgSz,
                                                    idx, recipFound);
    }

    /* if we don't have either option, malformed CMS */
    if (ret != 0)
        return ret;

    /* remove EncryptedKey */
    if ( (pkiMsgSz > (*idx + 1)) &&
         (pkiMsg[(*idx)++] != ASN_OCTET_STRING) )
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* put encrypted CEK in decryptedKey buffer for now, decrypt later */
    if (length > *encryptedKeySz)
        return BUFFER_E;

    XMEMCPY(encryptedKey, pkiMsg + (*idx), length);
    *encryptedKeySz = length;
    (*idx) += length;

    return 0;
}

#endif /* HAVE_ECC */


/* decode ASN.1 KeyAgreeRecipientInfo (kari), return 0 on success,
 * < 0 on error */
static int wc_PKCS7_DecodeKari(PKCS7* pkcs7, byte* pkiMsg, word32 pkiMsgSz,
                               word32* idx, byte* decryptedKey,
                               word32* decryptedKeySz, int* recipFound)
{
#ifdef HAVE_ECC
    int ret, keySz;
    int encryptedKeySz;
    int direction = 0;
    word32 keyAgreeOID, keyWrapOID;

#ifdef WOLFSSL_SMALL_STACK
    byte* encryptedKey;
#else
    byte encryptedKey[MAX_ENCRYPTED_KEY_SZ];
#endif

    WC_PKCS7_KARI* kari;

    if (pkcs7 == NULL || pkcs7->singleCert == NULL ||
        pkcs7->singleCertSz == 0 || pkiMsg == NULL ||
        idx == NULL || decryptedKey == NULL || decryptedKeySz == NULL) {
        return BAD_FUNC_ARG;
    }

    kari = wc_PKCS7_KariNew(pkcs7, WC_PKCS7_DECODE);
    if (kari == NULL)
        return MEMORY_E;

#ifdef WOLFSSL_SMALL_STACK
    encryptedKey = (byte*)XMALLOC(MAX_ENCRYPTED_KEY_SZ, NULL,
                                  DYNAMIC_TYPE_PKCS7);
    if (encryptedKey == NULL) {
        wc_PKCS7_KariFree(kari);
        return MEMORY_E;
    }
#endif
    encryptedKeySz = MAX_ENCRYPTED_KEY_SZ;

    /* parse cert and key */
    ret = wc_PKCS7_KariParseRecipCert(kari, (byte*)pkcs7->singleCert,
                                      pkcs7->singleCertSz, pkcs7->privateKey,
                                      pkcs7->privateKeySz);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* remove OriginatorIdentifierOrKey */
    ret = wc_PKCS7_KariGetOriginatorIdentifierOrKey(kari, pkiMsg,
                                                    pkiMsgSz, idx);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* try and remove optional UserKeyingMaterial */
    ret = wc_PKCS7_KariGetUserKeyingMaterial(kari, pkiMsg, pkiMsgSz, idx);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* remove KeyEncryptionAlgorithmIdentifier */
    ret = wc_PKCS7_KariGetKeyEncryptionAlgorithmId(kari, pkiMsg, pkiMsgSz,
                                                   idx, &keyAgreeOID,
                                                   &keyWrapOID);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* if user has not explicitly set keyAgreeOID, set from one in bundle */
    if (pkcs7->keyAgreeOID == 0)
        pkcs7->keyAgreeOID = keyAgreeOID;

    /* set direction based on key wrap algorithm */
    switch (keyWrapOID) {
#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
        case AES128_WRAP:
    #endif
    #ifdef WOLFSSL_AES_192
        case AES192_WRAP:
    #endif
    #ifdef WOLFSSL_AES_256
        case AES256_WRAP:
    #endif
            direction = AES_DECRYPTION;
            break;
#endif
        default:
            wc_PKCS7_KariFree(kari);
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
            #endif
            WOLFSSL_MSG("AES key wrap algorithm unsupported");
            return BAD_KEYWRAP_ALG_E;
    }

    /* remove RecipientEncryptedKeys */
    ret = wc_PKCS7_KariGetRecipientEncryptedKeys(kari, pkiMsg, pkiMsgSz,
                               idx, recipFound, encryptedKey, &encryptedKeySz);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* create KEK */
    ret = wc_PKCS7_KariGenerateKEK(kari, keyWrapOID, pkcs7->keyAgreeOID);
    if (ret != 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return ret;
    }

    /* decrypt CEK with KEK */
    keySz = wc_PKCS7_KariKeyWrap(encryptedKey, encryptedKeySz, kari->kek,
                                 kari->kekSz, decryptedKey, *decryptedKeySz,
                                 keyWrapOID, direction);
    if (keySz <= 0) {
        wc_PKCS7_KariFree(kari);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
        #endif
        return keySz;
    }
    *decryptedKeySz = (word32)keySz;

    wc_PKCS7_KariFree(kari);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(encryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
    #endif

    return 0;
#else
    (void)pkcs7;
    (void)pkiMsg;
    (void)pkiMsgSz;
    (void)idx;
    (void)decryptedKey;
    (void)decryptedKeySz;
    (void)recipFound;

    return NOT_COMPILED_IN;
#endif /* HAVE_ECC */
}


/* decode ASN.1 RecipientInfos SET, return 0 on success, < 0 on error */
static int wc_PKCS7_DecodeRecipientInfos(PKCS7* pkcs7, byte* pkiMsg,
                            word32 pkiMsgSz, word32* idx, byte* decryptedKey,
                            word32* decryptedKeySz, int* recipFound)
{
    word32 savedIdx;
    int version, ret, length;

    if (pkcs7 == NULL || pkiMsg == NULL || idx == NULL ||
        decryptedKey == NULL || decryptedKeySz == NULL ||
        recipFound == NULL) {
        return BAD_FUNC_ARG;
    }

    savedIdx = *idx;

    /* when looking for next recipient, use first sequence and version to
     * indicate there is another, if not, move on */
    while(*recipFound == 0) {

        /* remove RecipientInfo, if we don't have a SEQUENCE, back up idx to
         * last good saved one */
        if (GetSequence(pkiMsg, idx, &length, pkiMsgSz) > 0) {

            if (GetMyVersion(pkiMsg, idx, &version, pkiMsgSz) < 0) {
                *idx = savedIdx;
                break;
            }

            if (version != 0)
                return ASN_VERSION_E;

        #ifndef NO_RSA
            /* found ktri */
            ret = wc_PKCS7_DecodeKtri(pkcs7, pkiMsg, pkiMsgSz, idx,
                                      decryptedKey, decryptedKeySz,
                                      recipFound);
            if (ret != 0)
                return ret;
        #else
            return NOT_COMPILED_IN;
        #endif
        }
        else {
            /* kari is IMPLICIT[1] */
            *idx = savedIdx;
            if (pkiMsg[*idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1)) {
                (*idx)++;

                if (GetLength(pkiMsg, idx, &length, pkiMsgSz) < 0)
                    return ASN_PARSE_E;

                if (GetMyVersion(pkiMsg, idx, &version, pkiMsgSz) < 0) {
                    *idx = savedIdx;
                    break;
                }

                if (version != 3)
                    return ASN_VERSION_E;

                /* found kari */
                ret = wc_PKCS7_DecodeKari(pkcs7, pkiMsg, pkiMsgSz, idx,
                                          decryptedKey, decryptedKeySz,
                                          recipFound);
                if (ret != 0)
                    return ret;
            }
            else {
                /* failed to find RecipientInfo, restore idx and continue */
                *idx = savedIdx;
                break;
            }
        }

        /* update good idx */
        savedIdx = *idx;
    }

    return 0;
}


/* unwrap and decrypt PKCS#7 envelopedData object, return decoded size */
WOLFSSL_API int wc_PKCS7_DecodeEnvelopedData(PKCS7* pkcs7, byte* pkiMsg,
                                         word32 pkiMsgSz, byte* output,
                                         word32 outputSz)
{
    int recipFound = 0;
    int ret, version, length;
    word32 idx = 0;
    word32 contentType, encOID;
    word32 decryptedKeySz;

    int expBlockSz, blockKeySz;
    byte tmpIv[MAX_CONTENT_IV_SIZE];

#ifdef WOLFSSL_SMALL_STACK
    byte* decryptedKey;
#else
    byte decryptedKey[MAX_ENCRYPTED_KEY_SZ];
#endif
    int encryptedContentSz;
    byte padLen;
    byte* encryptedContent = NULL;
    int explicitOctet;

    if (pkcs7 == NULL || pkcs7->singleCert == NULL ||
        pkcs7->singleCertSz == 0)
        return BAD_FUNC_ARG;

    if (pkiMsg == NULL || pkiMsgSz == 0 ||
        output == NULL || outputSz == 0)
        return BAD_FUNC_ARG;

    /* read past ContentInfo, verify type is envelopedData */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (length == 0 && pkiMsg[idx-1] == 0x80) {
#ifdef ASN_BER_TO_DER
        word32 len = 0;

        ret = wc_BerToDer(pkiMsg, pkiMsgSz, NULL, &len);
        if (ret != LENGTH_ONLY_E)
            return ret;
        pkcs7->der = (byte*)XMALLOC(len, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        if (pkcs7->der == NULL)
            return MEMORY_E;
        len = 0;
        ret = wc_BerToDer(pkiMsg, pkiMsgSz, pkcs7->der, &len);
        if (ret < 0)
            return ret;

        pkiMsg = pkcs7->der;
        pkiMsgSz = len;
        idx = 0;
        if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
            return ASN_PARSE_E;
#else
        return BER_INDEF_E;
#endif
    }

    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (contentType != ENVELOPED_DATA) {
        WOLFSSL_MSG("PKCS#7 input not of type EnvelopedData");
        return PKCS7_OID_E;
    }

    if (pkiMsg[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0))
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* remove EnvelopedData and version */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (GetMyVersion(pkiMsg, &idx, &version, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* TODO :: make this more accurate */
    if ((pkcs7->publicKeyOID == RSAk && version != 0)
    #ifdef HAVE_ECC
            || (pkcs7->publicKeyOID == ECDSAk && version != 2)
    #endif
            ) {
        WOLFSSL_MSG("PKCS#7 envelopedData needs to be of version 0");
        return ASN_VERSION_E;
    }

    /* walk through RecipientInfo set, find correct recipient */
    if (GetSet(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

#ifdef WOLFSSL_SMALL_STACK
    decryptedKey = (byte*)XMALLOC(MAX_ENCRYPTED_KEY_SZ, NULL,
                                                       DYNAMIC_TYPE_PKCS7);
    if (decryptedKey == NULL)
        return MEMORY_E;
#endif
    decryptedKeySz = MAX_ENCRYPTED_KEY_SZ;

    ret = wc_PKCS7_DecodeRecipientInfos(pkcs7, pkiMsg, pkiMsgSz, &idx,
                                        decryptedKey, &decryptedKeySz,
                                        &recipFound);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ret;
    }

    if (recipFound == 0) {
        WOLFSSL_MSG("No recipient found in envelopedData that matches input");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return PKCS7_RECIP_E;
    }

    /* remove EncryptedContentInfo */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    if (GetAlgoId(pkiMsg, &idx, &encOID, oidBlkType, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    blockKeySz = wc_PKCS7_GetOIDKeySize(encOID);
    if (blockKeySz < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return blockKeySz;
    }

    expBlockSz = wc_PKCS7_GetOIDBlockSize(encOID);
    if (expBlockSz < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return expBlockSz;
    }

    /* get block cipher IV, stored in OPTIONAL parameter of AlgoID */
    if (pkiMsg[idx++] != ASN_OCTET_STRING) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    if (length != expBlockSz) {
        WOLFSSL_MSG("Incorrect IV length, must be of content alg block size");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    XMEMCPY(tmpIv, &pkiMsg[idx], length);
    idx += length;

    explicitOctet = pkiMsg[idx] == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 0);

    /* read encryptedContent, cont[0] */
    if (pkiMsg[idx] != (ASN_CONTEXT_SPECIFIC | 0) &&
        pkiMsg[idx] != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 0)) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }
    idx++;

    if (GetLength(pkiMsg, &idx, &encryptedContentSz, pkiMsgSz) <= 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ASN_PARSE_E;
    }

    if (explicitOctet) {
        if (pkiMsg[idx++] != ASN_OCTET_STRING) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
            return ASN_PARSE_E;
        }

        if (GetLength(pkiMsg, &idx, &encryptedContentSz, pkiMsgSz) <= 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
            return ASN_PARSE_E;
        }
    }

    encryptedContent = (byte*)XMALLOC(encryptedContentSz, pkcs7->heap,
                                                       DYNAMIC_TYPE_PKCS7);
    if (encryptedContent == NULL) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return MEMORY_E;
    }

    XMEMCPY(encryptedContent, &pkiMsg[idx], encryptedContentSz);

    /* decrypt encryptedContent */
    ret = wc_PKCS7_DecryptContent(encOID, decryptedKey, blockKeySz,
                                  tmpIv, expBlockSz, encryptedContent,
                                  encryptedContentSz, encryptedContent);
    if (ret != 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif
        return ret;
    }

    padLen = encryptedContent[encryptedContentSz-1];

    /* copy plaintext to output */
    XMEMCPY(output, encryptedContent, encryptedContentSz - padLen);

    /* free memory, zero out keys */
    ForceZero(decryptedKey, MAX_ENCRYPTED_KEY_SZ);
    ForceZero(encryptedContent, encryptedContentSz);
    XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(decryptedKey, NULL, DYNAMIC_TYPE_PKCS7);
#endif

    return encryptedContentSz - padLen;
}


#ifndef NO_PKCS7_ENCRYPTED_DATA

/* build PKCS#7 encryptedData content type, return encrypted size */
int wc_PKCS7_EncodeEncryptedData(PKCS7* pkcs7, byte* output, word32 outputSz)
{
    int ret, idx = 0;
    int totalSz, padSz, encryptedOutSz;

    int contentInfoSeqSz, outerContentTypeSz, outerContentSz;
    byte contentInfoSeq[MAX_SEQ_SZ];
    byte outerContentType[MAX_ALGO_SZ];
    byte outerContent[MAX_SEQ_SZ];

    int encDataSeqSz, verSz, blockSz;
    byte encDataSeq[MAX_SEQ_SZ];
    byte ver[MAX_VERSION_SZ];

    byte* plain = NULL;
    byte* encryptedContent = NULL;

    int encContentOctetSz, encContentSeqSz, contentTypeSz;
    int contentEncAlgoSz, ivOctetStringSz;
    byte encContentSeq[MAX_SEQ_SZ];
    byte contentType[MAX_ALGO_SZ];
    byte contentEncAlgo[MAX_ALGO_SZ];
    byte tmpIv[MAX_CONTENT_IV_SIZE];
    byte ivOctetString[MAX_OCTET_STR_SZ];
    byte encContentOctet[MAX_OCTET_STR_SZ];

    byte attribSet[MAX_SET_SZ];
    EncodedAttrib* attribs = NULL;
    word32 attribsSz;
    word32 attribsCount;
    word32 attribsSetSz;

    byte* flatAttribs = NULL;

    if (pkcs7 == NULL || pkcs7->content == NULL || pkcs7->contentSz == 0 ||
        pkcs7->encryptOID == 0 || pkcs7->encryptionKey == NULL ||
        pkcs7->encryptionKeySz == 0)
        return BAD_FUNC_ARG;

    if (output == NULL || outputSz == 0)
        return BAD_FUNC_ARG;

    /* outer content type */
    outerContentTypeSz = wc_SetContentType(ENCRYPTED_DATA, outerContentType);

    /* version, 2 if unprotectedAttrs present, 0 if absent */
    if (pkcs7->unprotectedAttribsSz > 0) {
        verSz = SetMyVersion(2, ver, 0);
    } else {
        verSz = SetMyVersion(0, ver, 0);
    }

    /* EncryptedContentInfo */
    contentTypeSz = wc_SetContentType(pkcs7->contentOID, contentType);
    if (contentTypeSz == 0)
        return BAD_FUNC_ARG;

    /* allocate encrypted content buffer, do PKCS#7 padding */
    blockSz = wc_PKCS7_GetOIDBlockSize(pkcs7->encryptOID);
    if (blockSz < 0)
        return blockSz;

    padSz = wc_PKCS7_GetPadSize(pkcs7->contentSz, blockSz);
    if (padSz < 0)
        return padSz;

    encryptedOutSz = pkcs7->contentSz + padSz;

    plain = (byte*)XMALLOC(encryptedOutSz, pkcs7->heap,
                           DYNAMIC_TYPE_PKCS7);
    if (plain == NULL)
        return MEMORY_E;

    ret = wc_PKCS7_PadData(pkcs7->content, pkcs7->contentSz, plain,
                           encryptedOutSz, blockSz);
    if (ret < 0) {
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    encryptedContent = (byte*)XMALLOC(encryptedOutSz, pkcs7->heap,
                                      DYNAMIC_TYPE_PKCS7);
    if (encryptedContent == NULL) {
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return MEMORY_E;
    }

    /* put together IV OCTET STRING */
    ivOctetStringSz = SetOctetString(blockSz, ivOctetString);

    /* build up ContentEncryptionAlgorithmIdentifier sequence,
       adding (ivOctetStringSz + blockSz) for IV OCTET STRING */
    contentEncAlgoSz = SetAlgoID(pkcs7->encryptOID, contentEncAlgo,
                                 oidBlkType, ivOctetStringSz + blockSz);
    if (contentEncAlgoSz == 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return BAD_FUNC_ARG;
    }

    /* encrypt content */
    ret = wc_PKCS7_GenerateIV(pkcs7, NULL, tmpIv, blockSz);
    if (ret != 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    ret = wc_PKCS7_EncryptContent(pkcs7->encryptOID, pkcs7->encryptionKey,
            pkcs7->encryptionKeySz, tmpIv, blockSz, plain, encryptedOutSz,
            encryptedContent);
    if (ret != 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    encContentOctetSz = SetImplicit(ASN_OCTET_STRING, 0,
                                    encryptedOutSz, encContentOctet);

    encContentSeqSz = SetSequence(contentTypeSz + contentEncAlgoSz +
                                  ivOctetStringSz + blockSz +
                                  encContentOctetSz + encryptedOutSz,
                                  encContentSeq);

    /* optional UnprotectedAttributes */
    if (pkcs7->unprotectedAttribsSz != 0) {

        if (pkcs7->unprotectedAttribs == NULL) {
            XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return BAD_FUNC_ARG;
        }

        attribs = (EncodedAttrib*)XMALLOC(
                sizeof(EncodedAttrib) * pkcs7->unprotectedAttribsSz,
                pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        if (attribs == NULL) {
            XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return MEMORY_E;
        }

        attribsCount = pkcs7->unprotectedAttribsSz;
        attribsSz = EncodeAttributes(attribs, pkcs7->unprotectedAttribsSz,
                                     pkcs7->unprotectedAttribs,
                                     pkcs7->unprotectedAttribsSz);

        flatAttribs = (byte*)XMALLOC(attribsSz, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        if (flatAttribs == NULL) {
            XFREE(attribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return MEMORY_E;
        }

        FlattenAttributes(flatAttribs, attribs, attribsCount);
        attribsSetSz = SetImplicit(ASN_SET, 1, attribsSz, attribSet);

    } else {
        attribsSz = 0;
        attribsSetSz = 0;
    }

    /* keep track of sizes for outer wrapper layering */
    totalSz = verSz + encContentSeqSz + contentTypeSz + contentEncAlgoSz +
              ivOctetStringSz + blockSz + encContentOctetSz + encryptedOutSz +
              attribsSz + attribsSetSz;;

    /* EncryptedData */
    encDataSeqSz = SetSequence(totalSz, encDataSeq);
    totalSz += encDataSeqSz;

    /* outer content */
    outerContentSz = SetExplicit(0, totalSz, outerContent);
    totalSz += outerContentTypeSz;
    totalSz += outerContentSz;

    /* ContentInfo */
    contentInfoSeqSz = SetSequence(totalSz, contentInfoSeq);
    totalSz += contentInfoSeqSz;

    if (totalSz > (int)outputSz) {
        WOLFSSL_MSG("PKCS#7 output buffer too small");
        if (pkcs7->unprotectedAttribsSz != 0) {
            XFREE(attribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            XFREE(flatAttribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        }
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return BUFFER_E;
    }

    XMEMCPY(output + idx, contentInfoSeq, contentInfoSeqSz);
    idx += contentInfoSeqSz;
    XMEMCPY(output + idx, outerContentType, outerContentTypeSz);
    idx += outerContentTypeSz;
    XMEMCPY(output + idx, outerContent, outerContentSz);
    idx += outerContentSz;
    XMEMCPY(output + idx, encDataSeq, encDataSeqSz);
    idx += encDataSeqSz;
    XMEMCPY(output + idx, ver, verSz);
    idx += verSz;
    XMEMCPY(output + idx, encContentSeq, encContentSeqSz);
    idx += encContentSeqSz;
    XMEMCPY(output + idx, contentType, contentTypeSz);
    idx += contentTypeSz;
    XMEMCPY(output + idx, contentEncAlgo, contentEncAlgoSz);
    idx += contentEncAlgoSz;
    XMEMCPY(output + idx, ivOctetString, ivOctetStringSz);
    idx += ivOctetStringSz;
    XMEMCPY(output + idx, tmpIv, blockSz);
    idx += blockSz;
    XMEMCPY(output + idx, encContentOctet, encContentOctetSz);
    idx += encContentOctetSz;
    XMEMCPY(output + idx, encryptedContent, encryptedOutSz);
    idx += encryptedOutSz;

    if (pkcs7->unprotectedAttribsSz != 0) {
        XMEMCPY(output + idx, attribSet, attribsSetSz);
        idx += attribsSetSz;
        XMEMCPY(output + idx, flatAttribs, attribsSz);
        idx += attribsSz;
        XFREE(attribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        XFREE(flatAttribs, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
    }

    XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
    XFREE(plain, pkcs7->heap, DYNAMIC_TYPE_PKCS7);

    return idx;
}


/* decode and store unprotected attributes in PKCS7->decodedAttrib. Return
 * 0 on success, negative on error. User must call wc_PKCS7_Free(). */
static int wc_PKCS7_DecodeUnprotectedAttributes(PKCS7* pkcs7, byte* pkiMsg,
                                             word32 pkiMsgSz, word32* inOutIdx)
{
    int ret, attribLen;
    word32 idx;

    if (pkcs7 == NULL || pkiMsg == NULL ||
        pkiMsgSz == 0 || inOutIdx == NULL)
        return BAD_FUNC_ARG;

    idx = *inOutIdx;

    if (pkiMsg[idx] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1))
        return ASN_PARSE_E;
    idx++;

    if (GetLength(pkiMsg, &idx, &attribLen, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* loop through attributes */
    if ((ret = wc_PKCS7_ParseAttribs(pkcs7, pkiMsg + idx, attribLen)) < 0) {
        return ret;
    }

    *inOutIdx = idx;

    return 0;
}


/* unwrap and decrypt PKCS#7/CMS encrypted-data object, returned decoded size */
int wc_PKCS7_DecodeEncryptedData(PKCS7* pkcs7, byte* pkiMsg, word32 pkiMsgSz,
                                 byte* output, word32 outputSz)
{
    int ret, version, length, haveAttribs;
    word32 idx = 0;
    word32 contentType, encOID;

    int expBlockSz;
    byte tmpIv[MAX_CONTENT_IV_SIZE];

    int encryptedContentSz;
    byte padLen;
    byte* encryptedContent = NULL;

    if (pkcs7 == NULL || pkcs7->encryptionKey == NULL ||
        pkcs7->encryptionKeySz == 0)
        return BAD_FUNC_ARG;

    if (pkiMsg == NULL || pkiMsgSz == 0 ||
        output == NULL || outputSz == 0)
        return BAD_FUNC_ARG;
    /* read past ContentInfo, verify type is encrypted-data */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (contentType != ENCRYPTED_DATA) {
        WOLFSSL_MSG("PKCS#7 input not of type EncryptedData");
        return PKCS7_OID_E;
    }

    if (pkiMsg[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0))
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* remove EncryptedData and version */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* get version, check later */
    haveAttribs = 0;
    if (GetMyVersion(pkiMsg, &idx, &version, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    /* remove EncryptedContentInfo */
    if (GetSequence(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (wc_GetContentType(pkiMsg, &idx, &contentType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (GetAlgoId(pkiMsg, &idx, &encOID, oidBlkType, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    expBlockSz = wc_PKCS7_GetOIDBlockSize(encOID);
    if (expBlockSz < 0)
        return expBlockSz;

    /* get block cipher IV, stored in OPTIONAL parameter of AlgoID */
    if (pkiMsg[idx++] != ASN_OCTET_STRING)
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, &idx, &length, pkiMsgSz) < 0)
        return ASN_PARSE_E;

    if (length != expBlockSz) {
        WOLFSSL_MSG("Incorrect IV length, must be of content alg block size");
        return ASN_PARSE_E;
    }

    XMEMCPY(tmpIv, &pkiMsg[idx], length);
    idx += length;

    /* read encryptedContent, cont[0] */
    if (pkiMsg[idx++] != (ASN_CONTEXT_SPECIFIC | 0))
        return ASN_PARSE_E;

    if (GetLength(pkiMsg, &idx, &encryptedContentSz, pkiMsgSz) <= 0)
        return ASN_PARSE_E;

    encryptedContent = (byte*)XMALLOC(encryptedContentSz, pkcs7->heap,
                                      DYNAMIC_TYPE_PKCS7);
    if (encryptedContent == NULL)
        return MEMORY_E;

    XMEMCPY(encryptedContent, &pkiMsg[idx], encryptedContentSz);
    idx += encryptedContentSz;

    /* decrypt encryptedContent */
    ret = wc_PKCS7_DecryptContent(encOID, pkcs7->encryptionKey,
                                  pkcs7->encryptionKeySz, tmpIv, expBlockSz,
                                  encryptedContent, encryptedContentSz,
                                  encryptedContent);
    if (ret != 0) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        return ret;
    }

    padLen = encryptedContent[encryptedContentSz-1];

    /* copy plaintext to output */
    XMEMCPY(output, encryptedContent, encryptedContentSz - padLen);

    /* get implicit[1] unprotected attributes, optional */
    pkcs7->decodedAttrib = NULL;
    if (idx < pkiMsgSz) {

        haveAttribs = 1;

        ret = wc_PKCS7_DecodeUnprotectedAttributes(pkcs7, pkiMsg,
                                                   pkiMsgSz, &idx);
        if (ret != 0) {
            ForceZero(encryptedContent, encryptedContentSz);
            XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
            return ASN_PARSE_E;
        }
    }

    /* go back and check the version now that attribs have been processed */
    if ((haveAttribs == 0 && version != 0) ||
        (haveAttribs == 1 && version != 2) ) {
        XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);
        WOLFSSL_MSG("Wrong PKCS#7 EncryptedData version");
        return ASN_VERSION_E;
    }

    ForceZero(encryptedContent, encryptedContentSz);
    XFREE(encryptedContent, pkcs7->heap, DYNAMIC_TYPE_PKCS7);

    return encryptedContentSz - padLen;
}

#endif /* NO_PKCS7_ENCRYPTED_DATA */

#else  /* HAVE_PKCS7 */


#ifdef _MSC_VER
    /* 4206 warning for blank file */
    #pragma warning(disable: 4206)
#endif


#endif /* HAVE_PKCS7 */

