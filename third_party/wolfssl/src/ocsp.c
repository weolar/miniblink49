/* ocsp.c
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


  /* Name change compatibility layer no longer needs to be included here */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCRYPT_ONLY
#ifdef HAVE_OCSP

#include <wolfssl/error-ssl.h>
#include <wolfssl/ocsp.h>
#include <wolfssl/internal.h>

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


int InitOCSP(WOLFSSL_OCSP* ocsp, WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("InitOCSP");

    ForceZero(ocsp, sizeof(WOLFSSL_OCSP));

    if (wc_InitMutex(&ocsp->ocspLock) != 0)
        return BAD_MUTEX_E;

    ocsp->cm = cm;

    return 0;
}


static int InitOcspEntry(OcspEntry* entry, OcspRequest* request)
{
    WOLFSSL_ENTER("InitOcspEntry");

    ForceZero(entry, sizeof(OcspEntry));

    XMEMCPY(entry->issuerHash,    request->issuerHash,    OCSP_DIGEST_SIZE);
    XMEMCPY(entry->issuerKeyHash, request->issuerKeyHash, OCSP_DIGEST_SIZE);

    return 0;
}


static void FreeOcspEntry(OcspEntry* entry, void* heap)
{
    CertStatus *status, *next;

    WOLFSSL_ENTER("FreeOcspEntry");

    for (status = entry->status; status; status = next) {
        next = status->next;

        if (status->rawOcspResponse)
            XFREE(status->rawOcspResponse, heap, DYNAMIC_TYPE_OCSP_STATUS);

        XFREE(status, heap, DYNAMIC_TYPE_OCSP_STATUS);
    }

    (void)heap;
}


void FreeOCSP(WOLFSSL_OCSP* ocsp, int dynamic)
{
    OcspEntry *entry, *next;

    WOLFSSL_ENTER("FreeOCSP");

    for (entry = ocsp->ocspList; entry; entry = next) {
        next = entry->next;
        FreeOcspEntry(entry, ocsp->cm->heap);
        XFREE(entry, ocsp->cm->heap, DYNAMIC_TYPE_OCSP_ENTRY);
    }

    wc_FreeMutex(&ocsp->ocspLock);

    if (dynamic)
        XFREE(ocsp, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);

}


static int xstat2err(int st)
{
    switch (st) {
        case CERT_GOOD:
            return 0;
        case CERT_REVOKED:
            return OCSP_CERT_REVOKED;
        default:
            return OCSP_CERT_UNKNOWN;
    }
}


int CheckCertOCSP(WOLFSSL_OCSP* ocsp, DecodedCert* cert, buffer* responseBuffer)
{
    int ret = OCSP_LOOKUP_FAIL;

#ifdef WOLFSSL_SMALL_STACK
    OcspRequest* ocspRequest;
#else
    OcspRequest ocspRequest[1];
#endif

    WOLFSSL_ENTER("CheckCertOCSP");


#ifdef WOLFSSL_SMALL_STACK
    ocspRequest = (OcspRequest*)XMALLOC(sizeof(OcspRequest), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (ocspRequest == NULL) {
        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_E;
    }
#endif

    if (InitOcspRequest(ocspRequest, cert, ocsp->cm->ocspSendNonce,
                                                         ocsp->cm->heap) == 0) {
        ret = CheckOcspRequest(ocsp, ocspRequest, responseBuffer);

        FreeOcspRequest(ocspRequest);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(ocspRequest, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    WOLFSSL_LEAVE("CheckCertOCSP", ret);
    return ret;
}

static int GetOcspEntry(WOLFSSL_OCSP* ocsp, OcspRequest* request,
                                                              OcspEntry** entry)
{
    WOLFSSL_ENTER("GetOcspEntry");

    *entry = NULL;

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        WOLFSSL_LEAVE("CheckCertOCSP", BAD_MUTEX_E);
        return BAD_MUTEX_E;
    }

    for (*entry = ocsp->ocspList; *entry; *entry = (*entry)->next)
        if (XMEMCMP((*entry)->issuerHash,    request->issuerHash,
                                                         OCSP_DIGEST_SIZE) == 0
        &&  XMEMCMP((*entry)->issuerKeyHash, request->issuerKeyHash,
                                                         OCSP_DIGEST_SIZE) == 0)
            break;

    if (*entry == NULL) {
        *entry = (OcspEntry*)XMALLOC(sizeof(OcspEntry),
                                       ocsp->cm->heap, DYNAMIC_TYPE_OCSP_ENTRY);
        if (*entry) {
            InitOcspEntry(*entry, request);
            (*entry)->next = ocsp->ocspList;
            ocsp->ocspList = *entry;
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

    return *entry ? 0 : MEMORY_ERROR;
}


static int GetOcspStatus(WOLFSSL_OCSP* ocsp, OcspRequest* request,
                  OcspEntry* entry, CertStatus** status, buffer* responseBuffer)
{
    int ret = OCSP_INVALID_STATUS;

    WOLFSSL_ENTER("GetOcspStatus");

    *status = NULL;

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        WOLFSSL_LEAVE("CheckCertOCSP", BAD_MUTEX_E);
        return BAD_MUTEX_E;
    }

    for (*status = entry->status; *status; *status = (*status)->next)
        if ((*status)->serialSz == request->serialSz
        &&  !XMEMCMP((*status)->serial, request->serial, (*status)->serialSz))
            break;

    if (responseBuffer && *status && !(*status)->rawOcspResponse) {
        /* force fetching again */
        ret = OCSP_INVALID_STATUS;
    }
    else if (*status) {
#ifndef NO_ASN_TIME
        if (ValidateDate((*status)->thisDate, (*status)->thisDateFormat, BEFORE)
        &&  ((*status)->nextDate[0] != 0)
        &&  ValidateDate((*status)->nextDate, (*status)->nextDateFormat, AFTER))
#endif
        {
            ret = xstat2err((*status)->status);

            if (responseBuffer) {
                responseBuffer->buffer = (byte*)XMALLOC(
                   (*status)->rawOcspResponseSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);

                if (responseBuffer->buffer) {
                    responseBuffer->length = (*status)->rawOcspResponseSz;
                    XMEMCPY(responseBuffer->buffer,
                            (*status)->rawOcspResponse,
                            (*status)->rawOcspResponseSz);
                }
            }
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

    return ret;
}

/* Check that the response for validity. Store result in status.
 *
 * ocsp           Context object for OCSP status.
 * response       OCSP response message data.
 * responseSz     Length of OCSP response message data.
 * reponseBuffer  Buffer object to return the response with.
 * status         The certificate status object.
 * entry          The OCSP entry for this certificate.
 * returns OCSP_LOOKUP_FAIL when the response is bad and 0 otherwise.
 */
static int CheckResponse(WOLFSSL_OCSP* ocsp, byte* response, int responseSz,
                         buffer* responseBuffer, CertStatus* status,
                         OcspEntry* entry, OcspRequest* ocspRequest)
{
#ifdef WOLFSSL_SMALL_STACK
    CertStatus*   newStatus;
    OcspResponse* ocspResponse;
#else
    CertStatus    newStatus[1];
    OcspResponse  ocspResponse[1];
#endif
    int           ret;
    int           validated      = 0;    /* ocsp validation flag */

#ifdef WOLFSSL_SMALL_STACK
    newStatus = (CertStatus*)XMALLOC(sizeof(CertStatus), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    ocspResponse = (OcspResponse*)XMALLOC(sizeof(OcspResponse), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);

    if (newStatus == NULL || ocspResponse == NULL) {
        if (newStatus) XFREE(newStatus, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (ocspResponse) XFREE(ocspResponse, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_E;
    }
#endif
    XMEMSET(newStatus, 0, sizeof(CertStatus));

    InitOcspResponse(ocspResponse, newStatus, response, responseSz);
    ret = OcspResponseDecode(ocspResponse, ocsp->cm, ocsp->cm->heap, 0);
    if (ret != 0) {
        WOLFSSL_MSG("OcspResponseDecode failed");
        goto end;
    }

    if (ocspResponse->responseStatus != OCSP_SUCCESSFUL) {
        WOLFSSL_MSG("OcspResponse status bad");
        goto end;
    }
    if (ocspRequest != NULL) {
        ret = CompareOcspReqResp(ocspRequest, ocspResponse);
        if (ret != 0) {
            goto end;
        }
    }

    if (responseBuffer) {
        responseBuffer->buffer = (byte*)XMALLOC(responseSz, ocsp->cm->heap,
                                                DYNAMIC_TYPE_TMP_BUFFER);

        if (responseBuffer->buffer) {
            responseBuffer->length = responseSz;
            XMEMCPY(responseBuffer->buffer, response, responseSz);
        }
    }

    ret = xstat2err(ocspResponse->status->status);
    if (ret == 0) {
        validated = 1;
    }

    if (wc_LockMutex(&ocsp->ocspLock) != 0) {
        ret = BAD_MUTEX_E;
        goto end;
    }

    if (status != NULL) {
        if (status->rawOcspResponse) {
            XFREE(status->rawOcspResponse, ocsp->cm->heap,
                  DYNAMIC_TYPE_OCSP_STATUS);
        }

        /* Replace existing certificate entry with updated */
        XMEMCPY(status, newStatus, sizeof(CertStatus));
    }
    else {
        /* Save new certificate entry */
        status = (CertStatus*)XMALLOC(sizeof(CertStatus),
                                      ocsp->cm->heap, DYNAMIC_TYPE_OCSP_STATUS);
        if (status != NULL) {
            XMEMCPY(status, newStatus, sizeof(CertStatus));
            status->next  = entry->status;
            entry->status = status;
            entry->totalStatus++;
        }
    }

    if (status && responseBuffer && responseBuffer->buffer) {
        status->rawOcspResponse = (byte*)XMALLOC(responseBuffer->length,
                                                 ocsp->cm->heap,
                                                 DYNAMIC_TYPE_OCSP_STATUS);

        if (status->rawOcspResponse) {
            status->rawOcspResponseSz = responseBuffer->length;
            XMEMCPY(status->rawOcspResponse, responseBuffer->buffer,
                    responseBuffer->length);
        }
    }

    wc_UnLockMutex(&ocsp->ocspLock);

end:
    if (ret == 0 && validated == 1) {
        WOLFSSL_MSG("New OcspResponse validated");
    } else if (ret != OCSP_CERT_REVOKED) {
        ret = OCSP_LOOKUP_FAIL;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(newStatus,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(ocspResponse, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return ret;
}

/* 0 on success */
int CheckOcspRequest(WOLFSSL_OCSP* ocsp, OcspRequest* ocspRequest,
                                                      buffer* responseBuffer)
{
    OcspEntry*  entry          = NULL;
    CertStatus* status         = NULL;
    byte*       request        = NULL;
    int         requestSz      = 2048;
    int         responseSz     = 0;
    byte*       response       = NULL;
    const char* url            = NULL;
    int         urlSz          = 0;
    int         ret            = -1;

    WOLFSSL_ENTER("CheckOcspRequest");

    if (responseBuffer) {
        responseBuffer->buffer = NULL;
        responseBuffer->length = 0;
    }

    ret = GetOcspEntry(ocsp, ocspRequest, &entry);
    if (ret != 0)
        return ret;

    ret = GetOcspStatus(ocsp, ocspRequest, entry, &status, responseBuffer);
    if (ret != OCSP_INVALID_STATUS)
        return ret;

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    if (ocsp->statusCb != NULL && ocspRequest->ssl != NULL) {
        ret = ocsp->statusCb((WOLFSSL*)ocspRequest->ssl, ocsp->cm->ocspIOCtx);
        if (ret == 0) {
            ret = wolfSSL_get_ocsp_response((WOLFSSL*)ocspRequest->ssl,
                                            &response);
            ret = CheckResponse(ocsp, response, ret, responseBuffer, status,
                                entry, NULL);
            if (response != NULL)
                XFREE(response, NULL, DYNAMIC_TYPE_OPENSSL);
            return ret;
        }
        return OCSP_LOOKUP_FAIL;
    }
#endif

    if (ocsp->cm->ocspUseOverrideURL) {
        url = ocsp->cm->ocspOverrideURL;
        if (url != NULL && url[0] != '\0')
            urlSz = (int)XSTRLEN(url);
        else
            return OCSP_NEED_URL;
    }
    else if (ocspRequest->urlSz != 0 && ocspRequest->url != NULL) {
        url = (const char *)ocspRequest->url;
        urlSz = ocspRequest->urlSz;
    }
    else {
        /* cert doesn't have extAuthInfo, assuming CERT_GOOD */
        return 0;
    }

    request = (byte*)XMALLOC(requestSz, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);
    if (request == NULL) {
        WOLFSSL_LEAVE("CheckCertOCSP", MEMORY_ERROR);
        return MEMORY_ERROR;
    }

    requestSz = EncodeOcspRequest(ocspRequest, request, requestSz);
    if (requestSz > 0 && ocsp->cm->ocspIOCb) {
        responseSz = ocsp->cm->ocspIOCb(ocsp->cm->ocspIOCtx, url, urlSz,
                                        request, requestSz, &response);
    }
    if (responseSz == WOLFSSL_CBIO_ERR_WANT_READ) {
        ret = WANT_READ;
    }

    XFREE(request, ocsp->cm->heap, DYNAMIC_TYPE_OCSP);

    if (responseSz >= 0 && response) {
        ret = CheckResponse(ocsp, response, responseSz, responseBuffer, status,
                            entry, ocspRequest);
    }

    if (response != NULL && ocsp->cm->ocspRespFreeCb)
        ocsp->cm->ocspRespFreeCb(ocsp->cm->ocspIOCtx, response);

    WOLFSSL_LEAVE("CheckOcspRequest", ret);
    return ret;
}

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)

int wolfSSL_OCSP_resp_find_status(WOLFSSL_OCSP_BASICRESP *bs,
    WOLFSSL_OCSP_CERTID* id, int* status, int* reason,
    WOLFSSL_ASN1_TIME** revtime, WOLFSSL_ASN1_TIME** thisupd,
    WOLFSSL_ASN1_TIME** nextupd)
{
    if (bs == NULL || id == NULL)
        return SSL_FAILURE;

    /* Only supporting one certificate status in asn.c. */
    if (CompareOcspReqResp(id, bs) != 0)
        return SSL_FAILURE;

    if (status != NULL)
        *status = bs->status->status;
    if (thisupd != NULL)
        *thisupd = (WOLFSSL_ASN1_TIME*)bs->status->thisDateAsn;
    if (nextupd != NULL)
        *nextupd = (WOLFSSL_ASN1_TIME*)bs->status->nextDateAsn;

    /* TODO: Not needed for Nginx. */
    if (reason != NULL)
        *reason = 0;
    if (revtime != NULL)
        *revtime = NULL;

    return SSL_SUCCESS;
}

const char *wolfSSL_OCSP_cert_status_str(long s)
{
    switch (s) {
        case CERT_GOOD:
            return "good";
        case CERT_REVOKED:
            return "revoked";
        case CERT_UNKNOWN:
            return "unknown";
        default:
            return "(UNKNOWN)";
    }
}

int wolfSSL_OCSP_check_validity(WOLFSSL_ASN1_TIME* thisupd,
    WOLFSSL_ASN1_TIME* nextupd, long sec, long maxsec)
{
    (void)thisupd;
    (void)nextupd;
    (void)sec;
    (void)maxsec;
    /* Dates validated in DecodeSingleResponse. */
    return SSL_SUCCESS;
}

void wolfSSL_OCSP_CERTID_free(WOLFSSL_OCSP_CERTID* certId)
{
    FreeOcspRequest(certId);
    XFREE(certId, NULL, DYNAMIC_TYPE_OPENSSL);
}

WOLFSSL_OCSP_CERTID* wolfSSL_OCSP_cert_to_id(
    const WOLFSSL_EVP_MD *dgst, const WOLFSSL_X509 *subject,
    const WOLFSSL_X509 *issuer)
{
    WOLFSSL_OCSP_CERTID* certId;
    DecodedCert cert;
    WOLFSSL_CERT_MANAGER* cm;
    int ret;
    DerBuffer* derCert = NULL;

    (void)dgst;

    cm = wolfSSL_CertManagerNew();
    if (cm == NULL)
        return NULL;

    ret = AllocDer(&derCert, issuer->derCert->length,
        issuer->derCert->type, NULL);
    if (ret == 0) {
        /* AddCA() frees the buffer. */
        XMEMCPY(derCert->buffer, issuer->derCert->buffer,
                issuer->derCert->length);
        AddCA(cm, &derCert, WOLFSSL_USER_CA, 1);
    }

    certId = (WOLFSSL_OCSP_CERTID*)XMALLOC(sizeof(WOLFSSL_OCSP_CERTID), NULL,
                                           DYNAMIC_TYPE_OPENSSL);
    if (certId != NULL) {
        InitDecodedCert(&cert, subject->derCert->buffer,
                        subject->derCert->length, NULL);
        if (ParseCertRelative(&cert, CERT_TYPE, VERIFY_OCSP, cm) != 0) {
            XFREE(certId, NULL, DYNAMIC_TYPE_OPENSSL);
            certId = NULL;
        }
        else {
            ret = InitOcspRequest(certId, &cert, 0, NULL);
            if (ret != 0) {
                XFREE(certId, NULL, DYNAMIC_TYPE_OPENSSL);
                certId = NULL;
            }
        }
        FreeDecodedCert(&cert);
    }

    wolfSSL_CertManagerFree(cm);

    return certId;
}

void wolfSSL_OCSP_BASICRESP_free(WOLFSSL_OCSP_BASICRESP* basicResponse)
{
    wolfSSL_OCSP_RESPONSE_free(basicResponse);
}

/* Signature verified in DecodeBasicOcspResponse.
 * But no store available to verify certificate. */
int wolfSSL_OCSP_basic_verify(WOLFSSL_OCSP_BASICRESP *bs,
    STACK_OF(WOLFSSL_X509) *certs, WOLFSSL_X509_STORE *st, unsigned long flags)
{
    DecodedCert cert;
    int         ret = SSL_SUCCESS;

    (void)certs;

    if (flags & OCSP_NOVERIFY)
        return SSL_SUCCESS;

    InitDecodedCert(&cert, bs->cert, bs->certSz, NULL);
    if (ParseCertRelative(&cert, CERT_TYPE, VERIFY, st->cm) < 0)
        ret = SSL_FAILURE;
    FreeDecodedCert(&cert);

    return ret;
}

void wolfSSL_OCSP_RESPONSE_free(OcspResponse* response)
{
    if (response->status != NULL)
        XFREE(response->status, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (response->source != NULL)
        XFREE(response->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(response, NULL, DYNAMIC_TYPE_OPENSSL);
}

OcspResponse* wolfSSL_d2i_OCSP_RESPONSE_bio(WOLFSSL_BIO* bio,
    OcspResponse** response)
{
    byte*         data;
    byte*         p;
    int           len;
    int           dataAlloced = 0;
    OcspResponse* ret = NULL;

    if (bio == NULL)
        return NULL;

    if (bio->type == BIO_MEMORY) {
        len = wolfSSL_BIO_get_mem_data(bio, &data);
        if (len <= 0 || data == NULL) {
            return NULL;
        }
    }
    else if (bio->type == BIO_FILE) {
        long i;
        long l;

        i = XFTELL(bio->file);
        if (i < 0)
            return NULL;
        XFSEEK(bio->file, 0, SEEK_END);
        l = XFTELL(bio->file);
        if (l < 0)
            return NULL;
        XFSEEK(bio->file, i, SEEK_SET);

        /* check calulated length */
        if (l - i <= 0)
            return NULL;

        data = (byte*)XMALLOC(l - i, 0, DYNAMIC_TYPE_TMP_BUFFER);
        if (data == NULL)
            return NULL;
        dataAlloced = 1;

        len = wolfSSL_BIO_read(bio, (char *)data, (int)l);
    }
    else
        return NULL;

    if (len > 0) {
        p = data;
        ret = wolfSSL_d2i_OCSP_RESPONSE(response, (const unsigned char **)&p, len);
    }

    if (dataAlloced)
        XFREE(data, 0, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}

OcspResponse* wolfSSL_d2i_OCSP_RESPONSE(OcspResponse** response,
    const unsigned char** data, int len)
{
    OcspResponse *resp = NULL;
    word32 idx = 0;
    int length = 0;

    if (data == NULL)
        return NULL;

    if (response != NULL)
        resp = *response;
    if (resp == NULL) {
        resp = (OcspResponse*)XMALLOC(sizeof(OcspResponse), NULL,
                                      DYNAMIC_TYPE_OPENSSL);
        if (resp == NULL)
            return NULL;
        XMEMSET(resp, 0, sizeof(OcspResponse));
    }

    resp->source = (byte*)XMALLOC(len, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (resp->source == NULL) {
        XFREE(resp, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }
    resp->status = (CertStatus*)XMALLOC(sizeof(CertStatus), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (resp->status == NULL) {
        XFREE(resp->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(resp, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }

    XMEMCPY(resp->source, *data, len);
    resp->maxIdx = len;

    if (OcspResponseDecode(resp, NULL, NULL, 1) != 0) {
        wolfSSL_OCSP_RESPONSE_free(resp);
        return NULL;
    }

    if (GetSequence(*data, &idx, &length, len) >= 0)
        (*data) += idx + length;

    return resp;
}

int wolfSSL_i2d_OCSP_RESPONSE(OcspResponse* response,
    unsigned char** data)
{
    if (data == NULL)
        return response->maxIdx;

    XMEMCPY(*data, response->source, response->maxIdx);
    return response->maxIdx;
}

int wolfSSL_OCSP_response_status(OcspResponse *response)
{
    return response->responseStatus;
}

const char *wolfSSL_OCSP_response_status_str(long s)
{
    switch (s) {
        case OCSP_SUCCESSFUL:
            return "successful";
        case OCSP_MALFORMED_REQUEST:
            return "malformedrequest";
        case OCSP_INTERNAL_ERROR:
            return "internalerror";
        case OCSP_TRY_LATER:
            return "trylater";
        case OCSP_SIG_REQUIRED:
            return "sigrequired";
        case OCSP_UNAUTHROIZED:
            return "unauthorized";
        default:
            return "(UNKNOWN)";
    }
}

WOLFSSL_OCSP_BASICRESP* wolfSSL_OCSP_response_get1_basic(OcspResponse* response)
{
    WOLFSSL_OCSP_BASICRESP* bs;

    bs = (WOLFSSL_OCSP_BASICRESP*)XMALLOC(sizeof(WOLFSSL_OCSP_BASICRESP), NULL,
                                          DYNAMIC_TYPE_OPENSSL);
    if (bs == NULL)
        return NULL;

    XMEMCPY(bs, response, sizeof(OcspResponse));
    bs->status = (CertStatus*)XMALLOC(sizeof(CertStatus), NULL,
                                      DYNAMIC_TYPE_TMP_BUFFER);
    bs->source = (byte*)XMALLOC(bs->maxIdx, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (bs->status == NULL || bs->source == NULL) {
        if (bs->status) XFREE(bs->status, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (bs->source) XFREE(bs->source, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        wolfSSL_OCSP_RESPONSE_free(bs);
        bs = NULL;
    }
    else {
        XMEMCPY(bs->status, response->status, sizeof(CertStatus));
        XMEMCPY(bs->source, response->source, response->maxIdx);
    }
    return bs;
}

OcspRequest* wolfSSL_OCSP_REQUEST_new(void)
{
    OcspRequest* request;

    request = (OcspRequest*)XMALLOC(sizeof(OcspRequest), NULL,
                                    DYNAMIC_TYPE_OPENSSL);
    if (request != NULL)
        XMEMSET(request, 0, sizeof(OcspRequest));

    return request;
}

void wolfSSL_OCSP_REQUEST_free(OcspRequest* request)
{
    FreeOcspRequest(request);
    XFREE(request, NULL, DYNAMIC_TYPE_OPENSSL);
}

int wolfSSL_i2d_OCSP_REQUEST(OcspRequest* request, unsigned char** data)
{
    word32 size;

    size = EncodeOcspRequest(request, NULL, 0);
    if (size <= 0 || data == NULL)
        return size;

    return EncodeOcspRequest(request, *data, size);
}

WOLFSSL_OCSP_ONEREQ* wolfSSL_OCSP_request_add0_id(OcspRequest *req,
    WOLFSSL_OCSP_CERTID *cid)
{
    if (req == NULL || cid == NULL)
        return NULL;

    FreeOcspRequest(req);
    XMEMCPY(req, cid, sizeof(OcspRequest));

    if (cid->serial != NULL) {
        req->serial = (byte*)XMALLOC(cid->serialSz, NULL,
                                     DYNAMIC_TYPE_OCSP_REQUEST);
        req->url = (byte*)XMALLOC(cid->urlSz, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
        if (req->serial == NULL || req->url == NULL) {
            FreeOcspRequest(req);
            return NULL;
        }

        XMEMCPY(req->serial, cid->serial, cid->serialSz);
        XMEMCPY(req->url, cid->url, cid->urlSz);
    }

    wolfSSL_OCSP_REQUEST_free(cid);

    return req;
}

#endif

#else /* HAVE_OCSP */


#ifdef _MSC_VER
    /* 4206 warning for blank file */
    #pragma warning(disable: 4206)
#endif


#endif /* HAVE_OCSP */
#endif /* WOLFCRYPT_ONLY */

