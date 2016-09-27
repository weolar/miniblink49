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

#ifndef WebCryptoAlgorithm_h
#define WebCryptoAlgorithm_h

#include "WebCommon.h"
#include "WebPrivatePtr.h"

#if INSIDE_BLINK
#include "wtf/PassOwnPtr.h"
#endif

namespace blink {

enum WebCryptoOperation {
    WebCryptoOperationEncrypt,
    WebCryptoOperationDecrypt,
    WebCryptoOperationSign,
    WebCryptoOperationVerify,
    WebCryptoOperationDigest,
    WebCryptoOperationGenerateKey,
    WebCryptoOperationImportKey,
    WebCryptoOperationGetKeyLength,
    WebCryptoOperationDeriveBits,
    WebCryptoOperationWrapKey,
    WebCryptoOperationUnwrapKey,
    WebCryptoOperationLast = WebCryptoOperationUnwrapKey,
};

enum WebCryptoAlgorithmId {
    WebCryptoAlgorithmIdAesCbc,
    WebCryptoAlgorithmIdHmac,
    WebCryptoAlgorithmIdRsaSsaPkcs1v1_5,
    WebCryptoAlgorithmIdSha1,
    WebCryptoAlgorithmIdSha256,
    WebCryptoAlgorithmIdSha384,
    WebCryptoAlgorithmIdSha512,
    WebCryptoAlgorithmIdAesGcm,
    WebCryptoAlgorithmIdRsaOaep,
    WebCryptoAlgorithmIdAesCtr,
    WebCryptoAlgorithmIdAesKw,
    WebCryptoAlgorithmIdRsaPss,
    WebCryptoAlgorithmIdEcdsa,
    WebCryptoAlgorithmIdEcdh,
    WebCryptoAlgorithmIdHkdf,
    WebCryptoAlgorithmIdPbkdf2,
#if INSIDE_BLINK
    WebCryptoAlgorithmIdLast = WebCryptoAlgorithmIdPbkdf2,
#endif
};

enum WebCryptoNamedCurve {
    WebCryptoNamedCurveP256,
    WebCryptoNamedCurveP384,
    WebCryptoNamedCurveP521,
#if INSIDE_BLINK
    WebCryptoNamedCurveLast = WebCryptoNamedCurveP521,
#endif
};

enum WebCryptoAlgorithmParamsType {
    WebCryptoAlgorithmParamsTypeNone,
    WebCryptoAlgorithmParamsTypeAesCbcParams,
    WebCryptoAlgorithmParamsTypeAesKeyGenParams,
    WebCryptoAlgorithmParamsTypeHmacImportParams,
    WebCryptoAlgorithmParamsTypeHmacKeyGenParams,
    WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams,
    WebCryptoAlgorithmParamsTypeRsaHashedImportParams,
    WebCryptoAlgorithmParamsTypeAesGcmParams,
    WebCryptoAlgorithmParamsTypeRsaOaepParams,
    WebCryptoAlgorithmParamsTypeAesCtrParams,
    WebCryptoAlgorithmParamsTypeRsaPssParams,
    WebCryptoAlgorithmParamsTypeEcdsaParams,
    WebCryptoAlgorithmParamsTypeEcKeyGenParams,
    WebCryptoAlgorithmParamsTypeEcKeyImportParams,
    WebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams,
    WebCryptoAlgorithmParamsTypeAesDerivedKeyParams,
    WebCryptoAlgorithmParamsTypeHkdfParams,
    WebCryptoAlgorithmParamsTypePbkdf2Params,
};

struct WebCryptoAlgorithmInfo {
    typedef char ParamsTypeOrUndefined;
    static const ParamsTypeOrUndefined Undefined = -1;

    // The canonical (case-sensitive) name for the algorithm as a
    // null-terminated C-string literal.
    const char* name;

    // A map from the operation to the expected parameter type of the algorithm.
    // If an operation is not applicable for the algorithm, set to Undefined.
    const ParamsTypeOrUndefined operationToParamsType[WebCryptoOperationLast + 1];
};


class WebCryptoAesCbcParams;
class WebCryptoAesKeyGenParams;
class WebCryptoHmacImportParams;
class WebCryptoHmacKeyGenParams;
class WebCryptoAesGcmParams;
class WebCryptoRsaOaepParams;
class WebCryptoAesCtrParams;
class WebCryptoRsaHashedKeyGenParams;
class WebCryptoRsaHashedImportParams;
class WebCryptoRsaPssParams;
class WebCryptoEcdsaParams;
class WebCryptoEcKeyGenParams;
class WebCryptoEcKeyImportParams;
class WebCryptoEcdhKeyDeriveParams;
class WebCryptoAesDerivedKeyParams;
class WebCryptoHkdfParams;
class WebCryptoPbkdf2Params;

class WebCryptoAlgorithmParams;
class WebCryptoAlgorithmPrivate;

// The WebCryptoAlgorithm represents a normalized algorithm and its parameters.
//   * Immutable
//   * Threadsafe
//   * Copiable (cheaply)
//
// If WebCryptoAlgorithm "isNull()" then it is invalid to call any of the other
// methods on it (other than destruction, assignment, or isNull()).
class WebCryptoAlgorithm {
public:
#if INSIDE_BLINK
    WebCryptoAlgorithm() { }
    BLINK_PLATFORM_EXPORT WebCryptoAlgorithm(WebCryptoAlgorithmId, PassOwnPtr<WebCryptoAlgorithmParams>);
#endif

    BLINK_PLATFORM_EXPORT static WebCryptoAlgorithm createNull();
    BLINK_PLATFORM_EXPORT static WebCryptoAlgorithm adoptParamsAndCreate(WebCryptoAlgorithmId, WebCryptoAlgorithmParams*);

    // Returns a WebCryptoAlgorithmInfo for the algorithm with the given ID. If
    // the ID is invalid, return 0. The caller can assume the pointer will be
    // valid for the program's entire runtime.
    BLINK_PLATFORM_EXPORT static const WebCryptoAlgorithmInfo* lookupAlgorithmInfo(WebCryptoAlgorithmId);

    ~WebCryptoAlgorithm() { reset(); }

    WebCryptoAlgorithm(const WebCryptoAlgorithm& other) { assign(other); }
    WebCryptoAlgorithm& operator=(const WebCryptoAlgorithm& other)
    {
        assign(other);
        return *this;
    }

    BLINK_PLATFORM_EXPORT bool isNull() const;

    BLINK_PLATFORM_EXPORT WebCryptoAlgorithmId id() const;

    BLINK_PLATFORM_EXPORT WebCryptoAlgorithmParamsType paramsType() const;

    // Retrieves the type-specific parameters. The algorithm contains at most 1
    // type of parameters. Retrieving an invalid parameter will return 0.
    BLINK_PLATFORM_EXPORT const WebCryptoAesCbcParams* aesCbcParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoAesKeyGenParams* aesKeyGenParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoHmacImportParams* hmacImportParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoHmacKeyGenParams* hmacKeyGenParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoAesGcmParams* aesGcmParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoRsaOaepParams* rsaOaepParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoAesCtrParams* aesCtrParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoRsaHashedImportParams* rsaHashedImportParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoRsaHashedKeyGenParams* rsaHashedKeyGenParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoRsaPssParams* rsaPssParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoEcdsaParams* ecdsaParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoEcKeyGenParams* ecKeyGenParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoEcKeyImportParams* ecKeyImportParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoEcdhKeyDeriveParams* ecdhKeyDeriveParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoAesDerivedKeyParams* aesDerivedKeyParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoHkdfParams* hkdfParams() const;
    BLINK_PLATFORM_EXPORT const WebCryptoPbkdf2Params* pbkdf2Params() const;

    // Returns true if the provided algorithm ID is for a hash (in other words, SHA-*)
    BLINK_PLATFORM_EXPORT static bool isHash(WebCryptoAlgorithmId);
    // Returns true if the provided algorithm ID is for a key derivation function
    BLINK_PLATFORM_EXPORT static bool isKdf(WebCryptoAlgorithmId);

private:
    BLINK_PLATFORM_EXPORT void assign(const WebCryptoAlgorithm& other);
    BLINK_PLATFORM_EXPORT void reset();

    WebPrivatePtr<WebCryptoAlgorithmPrivate> m_private;
};

} // namespace blink

#endif
