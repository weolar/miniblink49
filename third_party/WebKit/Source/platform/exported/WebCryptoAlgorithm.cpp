/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/platform/WebCryptoAlgorithm.h"

#include "public/platform/WebCryptoAlgorithmParams.h"
#include "wtf/Assertions.h"
#include "wtf/OwnPtr.h"
#include "wtf/StdLibExtras.h"
#include "wtf/ThreadSafeRefCounted.h"

namespace blink {

namespace {

// A mapping from the algorithm ID to information about the algorithm.
const WebCryptoAlgorithmInfo algorithmIdToInfo[] = {
    { // Index 0
        "AES-CBC", {
            WebCryptoAlgorithmParamsTypeAesCbcParams, // Encrypt
            WebCryptoAlgorithmParamsTypeAesCbcParams, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeAesKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeAesDerivedKeyParams, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmParamsTypeAesCbcParams, // WrapKey
            WebCryptoAlgorithmParamsTypeAesCbcParams // UnwrapKey
        }
    }, { // Index 1
        "HMAC", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmParamsTypeNone, // Sign
            WebCryptoAlgorithmParamsTypeNone, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeHmacKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeHmacImportParams, // ImportKey
            WebCryptoAlgorithmParamsTypeHmacImportParams, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 2
        "RSASSA-PKCS1-v1_5", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmParamsTypeNone, // Sign
            WebCryptoAlgorithmParamsTypeNone, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeRsaHashedImportParams, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 3
        "SHA-1", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmParamsTypeNone, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmInfo::Undefined, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 4
        "SHA-256", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmParamsTypeNone, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmInfo::Undefined, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 5
        "SHA-384", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmParamsTypeNone, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmInfo::Undefined, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 6
        "SHA-512", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmParamsTypeNone, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmInfo::Undefined, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 7
        "AES-GCM", {
            WebCryptoAlgorithmParamsTypeAesGcmParams, // Encrypt
            WebCryptoAlgorithmParamsTypeAesGcmParams, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeAesKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeAesDerivedKeyParams, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmParamsTypeAesGcmParams, // WrapKey
            WebCryptoAlgorithmParamsTypeAesGcmParams // UnwrapKey
        }
    }, { // Index 8
        "RSA-OAEP", {
            WebCryptoAlgorithmParamsTypeRsaOaepParams, // Encrypt
            WebCryptoAlgorithmParamsTypeRsaOaepParams, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeRsaHashedImportParams, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmParamsTypeRsaOaepParams, // WrapKey
            WebCryptoAlgorithmParamsTypeRsaOaepParams // UnwrapKey
        }
    }, { // Index 9
        "AES-CTR", {
            WebCryptoAlgorithmParamsTypeAesCtrParams, // Encrypt
            WebCryptoAlgorithmParamsTypeAesCtrParams, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeAesKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeAesDerivedKeyParams, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmParamsTypeAesCtrParams, // WrapKey
            WebCryptoAlgorithmParamsTypeAesCtrParams // UnwrapKey
        }
    }, { // Index 10
        "AES-KW", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeAesKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeAesDerivedKeyParams, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmParamsTypeNone, // WrapKey
            WebCryptoAlgorithmParamsTypeNone // UnwrapKey
        }
    }, { // Index 11
        "RSA-PSS", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmParamsTypeRsaPssParams, // Sign
            WebCryptoAlgorithmParamsTypeRsaPssParams, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeRsaHashedImportParams, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 12
        "ECDSA", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmParamsTypeEcdsaParams, // Sign
            WebCryptoAlgorithmParamsTypeEcdsaParams, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeEcKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeEcKeyImportParams, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmInfo::Undefined, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 13
        "ECDH", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmParamsTypeEcKeyGenParams, // GenerateKey
            WebCryptoAlgorithmParamsTypeEcKeyImportParams, // ImportKey
            WebCryptoAlgorithmInfo::Undefined, // GetKeyLength
            WebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 14
        "HKDF", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeNone, // GetKeyLength
            WebCryptoAlgorithmParamsTypeHkdfParams, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    }, { // Index 15
        "PBKDF2", {
            WebCryptoAlgorithmInfo::Undefined, // Encrypt
            WebCryptoAlgorithmInfo::Undefined, // Decrypt
            WebCryptoAlgorithmInfo::Undefined, // Sign
            WebCryptoAlgorithmInfo::Undefined, // Verify
            WebCryptoAlgorithmInfo::Undefined, // Digest
            WebCryptoAlgorithmInfo::Undefined, // GenerateKey
            WebCryptoAlgorithmParamsTypeNone, // ImportKey
            WebCryptoAlgorithmParamsTypeNone, // GetKeyLength
            WebCryptoAlgorithmParamsTypePbkdf2Params, // DeriveBits
            WebCryptoAlgorithmInfo::Undefined, // WrapKey
            WebCryptoAlgorithmInfo::Undefined // UnwrapKey
        }
    },
};

// Initializing the algorithmIdToInfo table above depends on knowing the enum
// values for algorithm IDs. If those ever change, the table will need to be
// updated.
static_assert(WebCryptoAlgorithmIdAesCbc == 0, "AES CBC id must match");
static_assert(WebCryptoAlgorithmIdHmac == 1, "HMAC id must match");
static_assert(WebCryptoAlgorithmIdRsaSsaPkcs1v1_5 == 2, "RSASSA-PKCS1-v1_5 id must match");
static_assert(WebCryptoAlgorithmIdSha1 == 3, "SHA1 id must match");
static_assert(WebCryptoAlgorithmIdSha256 == 4, "SHA256 id must match");
static_assert(WebCryptoAlgorithmIdSha384 == 5, "SHA384 id must match");
static_assert(WebCryptoAlgorithmIdSha512 == 6, "SHA512 id must match");
static_assert(WebCryptoAlgorithmIdAesGcm == 7, "AES GCM id must match");
static_assert(WebCryptoAlgorithmIdRsaOaep == 8, "RSA OAEP id must match");
static_assert(WebCryptoAlgorithmIdAesCtr == 9, "AES CTR id must match");
static_assert(WebCryptoAlgorithmIdAesKw == 10, "AESKW id must match");
static_assert(WebCryptoAlgorithmIdRsaPss == 11, "RSA-PSS id must match");
static_assert(WebCryptoAlgorithmIdEcdsa == 12, "ECDSA id must match");
static_assert(WebCryptoAlgorithmIdEcdh == 13, "ECDH id must match");
static_assert(WebCryptoAlgorithmIdHkdf == 14, "HKDF id must match");
static_assert(WebCryptoAlgorithmIdPbkdf2 == 15, "Pbkdf2 id must match");
static_assert(WebCryptoAlgorithmIdLast == 15, "last id must match");
static_assert(10 == WebCryptoOperationLast, "the parameter mapping needs to be updated");

} // namespace

class WebCryptoAlgorithmPrivate : public ThreadSafeRefCounted<WebCryptoAlgorithmPrivate> {
public:
    WebCryptoAlgorithmPrivate(WebCryptoAlgorithmId id, PassOwnPtr<WebCryptoAlgorithmParams> params)
        : id(id)
        , params(params)
    {
    }

    WebCryptoAlgorithmId id;
    OwnPtr<WebCryptoAlgorithmParams> params;
};

WebCryptoAlgorithm::WebCryptoAlgorithm(WebCryptoAlgorithmId id, PassOwnPtr<WebCryptoAlgorithmParams> params)
    : m_private(adoptRef(new WebCryptoAlgorithmPrivate(id, params)))
{
}

WebCryptoAlgorithm WebCryptoAlgorithm::createNull()
{
    return WebCryptoAlgorithm();
}

WebCryptoAlgorithm WebCryptoAlgorithm::adoptParamsAndCreate(WebCryptoAlgorithmId id, WebCryptoAlgorithmParams* params)
{
    return WebCryptoAlgorithm(id, adoptPtr(params));
}

const WebCryptoAlgorithmInfo* WebCryptoAlgorithm::lookupAlgorithmInfo(WebCryptoAlgorithmId id)
{
    const unsigned idInt = id;
    if (idInt >= WTF_ARRAY_LENGTH(algorithmIdToInfo))
        return 0;
    return &algorithmIdToInfo[id];
}

bool WebCryptoAlgorithm::isNull() const
{
    return m_private.isNull();
}

WebCryptoAlgorithmId WebCryptoAlgorithm::id() const
{
    ASSERT(!isNull());
    return m_private->id;
}

WebCryptoAlgorithmParamsType WebCryptoAlgorithm::paramsType() const
{
    ASSERT(!isNull());
    if (!m_private->params)
        return WebCryptoAlgorithmParamsTypeNone;
    return m_private->params->type();
}

const WebCryptoAesCbcParams* WebCryptoAlgorithm::aesCbcParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeAesCbcParams)
        return static_cast<WebCryptoAesCbcParams*>(m_private->params.get());
    return 0;
}

const WebCryptoAesCtrParams* WebCryptoAlgorithm::aesCtrParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeAesCtrParams)
        return static_cast<WebCryptoAesCtrParams*>(m_private->params.get());
    return 0;
}

const WebCryptoAesKeyGenParams* WebCryptoAlgorithm::aesKeyGenParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeAesKeyGenParams)
        return static_cast<WebCryptoAesKeyGenParams*>(m_private->params.get());
    return 0;
}

const WebCryptoHmacImportParams* WebCryptoAlgorithm::hmacImportParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeHmacImportParams)
        return static_cast<WebCryptoHmacImportParams*>(m_private->params.get());
    return 0;
}

const WebCryptoHmacKeyGenParams* WebCryptoAlgorithm::hmacKeyGenParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeHmacKeyGenParams)
        return static_cast<WebCryptoHmacKeyGenParams*>(m_private->params.get());
    return 0;
}

const WebCryptoAesGcmParams* WebCryptoAlgorithm::aesGcmParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeAesGcmParams)
        return static_cast<WebCryptoAesGcmParams*>(m_private->params.get());
    return 0;
}

const WebCryptoRsaOaepParams* WebCryptoAlgorithm::rsaOaepParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeRsaOaepParams)
        return static_cast<WebCryptoRsaOaepParams*>(m_private->params.get());
    return 0;
}

const WebCryptoRsaHashedImportParams* WebCryptoAlgorithm::rsaHashedImportParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeRsaHashedImportParams)
        return static_cast<WebCryptoRsaHashedImportParams*>(m_private->params.get());
    return 0;
}

const WebCryptoRsaHashedKeyGenParams* WebCryptoAlgorithm::rsaHashedKeyGenParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams)
        return static_cast<WebCryptoRsaHashedKeyGenParams*>(m_private->params.get());
    return 0;
}

const WebCryptoRsaPssParams* WebCryptoAlgorithm::rsaPssParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeRsaPssParams)
        return static_cast<WebCryptoRsaPssParams*>(m_private->params.get());
    return 0;
}

const WebCryptoEcdsaParams* WebCryptoAlgorithm::ecdsaParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeEcdsaParams)
        return static_cast<WebCryptoEcdsaParams*>(m_private->params.get());
    return 0;
}

const WebCryptoEcKeyGenParams* WebCryptoAlgorithm::ecKeyGenParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeEcKeyGenParams)
        return static_cast<WebCryptoEcKeyGenParams*>(m_private->params.get());
    return 0;
}

const WebCryptoEcKeyImportParams* WebCryptoAlgorithm::ecKeyImportParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeEcKeyImportParams)
        return static_cast<WebCryptoEcKeyImportParams*>(m_private->params.get());
    return 0;
}

const WebCryptoEcdhKeyDeriveParams* WebCryptoAlgorithm::ecdhKeyDeriveParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams)
        return static_cast<WebCryptoEcdhKeyDeriveParams*>(m_private->params.get());
    return 0;
}

const WebCryptoAesDerivedKeyParams* WebCryptoAlgorithm::aesDerivedKeyParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeAesDerivedKeyParams)
        return static_cast<WebCryptoAesDerivedKeyParams*>(m_private->params.get());
    return 0;
}

const WebCryptoHkdfParams* WebCryptoAlgorithm::hkdfParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypeHkdfParams)
        return static_cast<WebCryptoHkdfParams*>(m_private->params.get());
    return 0;
}

const WebCryptoPbkdf2Params* WebCryptoAlgorithm::pbkdf2Params() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoAlgorithmParamsTypePbkdf2Params)
        return static_cast<WebCryptoPbkdf2Params*>(m_private->params.get());
    return 0;
}

bool WebCryptoAlgorithm::isHash(WebCryptoAlgorithmId id)
{
    switch (id) {
    case WebCryptoAlgorithmIdSha1:
    case WebCryptoAlgorithmIdSha256:
    case WebCryptoAlgorithmIdSha384:
    case WebCryptoAlgorithmIdSha512:
        return true;
    case WebCryptoAlgorithmIdAesCbc:
    case WebCryptoAlgorithmIdHmac:
    case WebCryptoAlgorithmIdRsaSsaPkcs1v1_5:
    case WebCryptoAlgorithmIdAesGcm:
    case WebCryptoAlgorithmIdRsaOaep:
    case WebCryptoAlgorithmIdAesCtr:
    case WebCryptoAlgorithmIdAesKw:
    case WebCryptoAlgorithmIdRsaPss:
    case WebCryptoAlgorithmIdEcdsa:
    case WebCryptoAlgorithmIdEcdh:
    case WebCryptoAlgorithmIdHkdf:
    case WebCryptoAlgorithmIdPbkdf2:
        break;
    }
    return false;
}

bool WebCryptoAlgorithm::isKdf(WebCryptoAlgorithmId id)
{
    switch (id) {
    case WebCryptoAlgorithmIdHkdf:
    case WebCryptoAlgorithmIdPbkdf2:
        return true;
    case WebCryptoAlgorithmIdSha1:
    case WebCryptoAlgorithmIdSha256:
    case WebCryptoAlgorithmIdSha384:
    case WebCryptoAlgorithmIdSha512:
    case WebCryptoAlgorithmIdAesCbc:
    case WebCryptoAlgorithmIdHmac:
    case WebCryptoAlgorithmIdRsaSsaPkcs1v1_5:
    case WebCryptoAlgorithmIdAesGcm:
    case WebCryptoAlgorithmIdRsaOaep:
    case WebCryptoAlgorithmIdAesCtr:
    case WebCryptoAlgorithmIdAesKw:
    case WebCryptoAlgorithmIdRsaPss:
    case WebCryptoAlgorithmIdEcdsa:
    case WebCryptoAlgorithmIdEcdh:
        break;
    }
    return false;
}

void WebCryptoAlgorithm::assign(const WebCryptoAlgorithm& other)
{
    m_private = other.m_private;
}

void WebCryptoAlgorithm::reset()
{
    m_private.reset();
}

} // namespace blink
