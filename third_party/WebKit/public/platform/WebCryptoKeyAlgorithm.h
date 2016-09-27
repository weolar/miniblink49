/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef WebCryptoKeyAlgorithm_h
#define WebCryptoKeyAlgorithm_h

#include "WebCommon.h"
#include "WebCryptoAlgorithm.h"
#include "WebCryptoKeyAlgorithmParams.h"
#include "WebPrivatePtr.h"

#if INSIDE_BLINK
#include "wtf/PassOwnPtr.h"
#endif

namespace blink {

class WebCryptoKeyAlgorithmPrivate;

// WebCryptoKeyAlgorithm represents the algorithm used to generate a key.
//   * Immutable
//   * Threadsafe
//   * Copiable (cheaply)
class WebCryptoKeyAlgorithm {
public:
    WebCryptoKeyAlgorithm() { }

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebCryptoKeyAlgorithm(WebCryptoAlgorithmId, PassOwnPtr<WebCryptoKeyAlgorithmParams>);
#endif

    // FIXME: Delete this in favor of the create*() functions.
    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm adoptParamsAndCreate(WebCryptoAlgorithmId, WebCryptoKeyAlgorithmParams*);

    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm createAes(WebCryptoAlgorithmId, unsigned short keyLengthBits);
    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm createHmac(WebCryptoAlgorithmId hash, unsigned keyLengthBits);
    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm createRsaHashed(WebCryptoAlgorithmId, unsigned modulusLengthBits, const unsigned char* publicExponent, unsigned publicExponentSize, WebCryptoAlgorithmId hash);
    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm createEc(WebCryptoAlgorithmId, WebCryptoNamedCurve);
    BLINK_PLATFORM_EXPORT static WebCryptoKeyAlgorithm createWithoutParams(WebCryptoAlgorithmId);

    ~WebCryptoKeyAlgorithm() { reset(); }

    WebCryptoKeyAlgorithm(const WebCryptoKeyAlgorithm& other) { assign(other); }
    WebCryptoKeyAlgorithm& operator=(const WebCryptoKeyAlgorithm& other)
    {
        assign(other);
        return *this;
    }

    BLINK_PLATFORM_EXPORT bool isNull() const;

    BLINK_PLATFORM_EXPORT WebCryptoAlgorithmId id() const;

    BLINK_PLATFORM_EXPORT WebCryptoKeyAlgorithmParamsType paramsType() const;

    // Returns the type-specific parameters for this key. If the requested
    // parameters are not applicable (for instance an HMAC key does not have
    // any AES parameters) then returns 0.
    BLINK_PLATFORM_EXPORT WebCryptoAesKeyAlgorithmParams* aesParams() const;
    BLINK_PLATFORM_EXPORT WebCryptoHmacKeyAlgorithmParams* hmacParams() const;
    BLINK_PLATFORM_EXPORT WebCryptoRsaHashedKeyAlgorithmParams* rsaHashedParams() const;
    BLINK_PLATFORM_EXPORT WebCryptoEcKeyAlgorithmParams* ecParams() const;

    // Write the algorithm parameters to a dictionary.
    BLINK_PLATFORM_EXPORT void writeToDictionary(WebCryptoKeyAlgorithmDictionary*) const;

private:
    BLINK_PLATFORM_EXPORT void assign(const WebCryptoKeyAlgorithm& other);
    BLINK_PLATFORM_EXPORT void reset();

    WebPrivatePtr<WebCryptoKeyAlgorithmPrivate> m_private;
};

} // namespace blink

#endif
