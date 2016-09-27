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

#include "config.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"

#include "wtf/OwnPtr.h"
#include "wtf/ThreadSafeRefCounted.h"

namespace blink {

// FIXME: Remove the need for this.
WebCryptoAlgorithm createHash(WebCryptoAlgorithmId hash)
{
    return WebCryptoAlgorithm::adoptParamsAndCreate(hash, 0);
}

class WebCryptoKeyAlgorithmPrivate : public ThreadSafeRefCounted<WebCryptoKeyAlgorithmPrivate> {
public:
    WebCryptoKeyAlgorithmPrivate(WebCryptoAlgorithmId id, PassOwnPtr<WebCryptoKeyAlgorithmParams> params)
        : id(id)
        , params(params)
    {
    }

    WebCryptoAlgorithmId id;
    OwnPtr<WebCryptoKeyAlgorithmParams> params;
};

WebCryptoKeyAlgorithm::WebCryptoKeyAlgorithm(WebCryptoAlgorithmId id, PassOwnPtr<WebCryptoKeyAlgorithmParams> params)
    : m_private(adoptRef(new WebCryptoKeyAlgorithmPrivate(id, params)))
{
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::adoptParamsAndCreate(WebCryptoAlgorithmId id, WebCryptoKeyAlgorithmParams* params)
{
    return WebCryptoKeyAlgorithm(id, adoptPtr(params));
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::createAes(WebCryptoAlgorithmId id, unsigned short keyLengthBits)
{
    // FIXME: Verify that id is an AES algorithm.
    // FIXME: Move this somewhere more general.
    if (keyLengthBits != 128 && keyLengthBits != 192 && keyLengthBits != 256)
        return WebCryptoKeyAlgorithm();
    return WebCryptoKeyAlgorithm(id, adoptPtr(new WebCryptoAesKeyAlgorithmParams(keyLengthBits)));
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::createHmac(WebCryptoAlgorithmId hash, unsigned keyLengthBits)
{
    if (!WebCryptoAlgorithm::isHash(hash))
        return WebCryptoKeyAlgorithm();
    return WebCryptoKeyAlgorithm(WebCryptoAlgorithmIdHmac, adoptPtr(new WebCryptoHmacKeyAlgorithmParams(createHash(hash), keyLengthBits)));
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::createRsaHashed(WebCryptoAlgorithmId id, unsigned modulusLengthBits, const unsigned char* publicExponent, unsigned publicExponentSize, WebCryptoAlgorithmId hash)
{
    // FIXME: Verify that id is an RSA algorithm which expects a hash
    if (!WebCryptoAlgorithm::isHash(hash))
        return WebCryptoKeyAlgorithm();
    return WebCryptoKeyAlgorithm(id, adoptPtr(new WebCryptoRsaHashedKeyAlgorithmParams(modulusLengthBits, publicExponent, publicExponentSize, createHash(hash))));
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::createEc(WebCryptoAlgorithmId id, WebCryptoNamedCurve namedCurve)
{
    return WebCryptoKeyAlgorithm(id, adoptPtr(new WebCryptoEcKeyAlgorithmParams(namedCurve)));
}

WebCryptoKeyAlgorithm WebCryptoKeyAlgorithm::createWithoutParams(WebCryptoAlgorithmId id)
{
    if (!WebCryptoAlgorithm::isKdf(id))
        return WebCryptoKeyAlgorithm();
    return WebCryptoKeyAlgorithm(id, nullptr);
}

bool WebCryptoKeyAlgorithm::isNull() const
{
    return m_private.isNull();
}

WebCryptoAlgorithmId WebCryptoKeyAlgorithm::id() const
{
    ASSERT(!isNull());
    return m_private->id;
}

WebCryptoKeyAlgorithmParamsType WebCryptoKeyAlgorithm::paramsType() const
{
    ASSERT(!isNull());
    if (!m_private->params.get())
        return WebCryptoKeyAlgorithmParamsTypeNone;
    return m_private->params->type();
}

WebCryptoAesKeyAlgorithmParams* WebCryptoKeyAlgorithm::aesParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoKeyAlgorithmParamsTypeAes)
        return static_cast<WebCryptoAesKeyAlgorithmParams*>(m_private->params.get());
    return 0;
}

WebCryptoHmacKeyAlgorithmParams* WebCryptoKeyAlgorithm::hmacParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoKeyAlgorithmParamsTypeHmac)
        return static_cast<WebCryptoHmacKeyAlgorithmParams*>(m_private->params.get());
    return 0;
}

WebCryptoRsaHashedKeyAlgorithmParams* WebCryptoKeyAlgorithm::rsaHashedParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoKeyAlgorithmParamsTypeRsaHashed)
        return static_cast<WebCryptoRsaHashedKeyAlgorithmParams*>(m_private->params.get());
    return 0;
}

WebCryptoEcKeyAlgorithmParams* WebCryptoKeyAlgorithm::ecParams() const
{
    ASSERT(!isNull());
    if (paramsType() == WebCryptoKeyAlgorithmParamsTypeEc)
        return static_cast<WebCryptoEcKeyAlgorithmParams*>(m_private->params.get());
    return 0;
}

void WebCryptoKeyAlgorithm::writeToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const
{
    ASSERT(!isNull());
    dict->setString("name", WebCryptoAlgorithm::lookupAlgorithmInfo(id())->name);
    if (m_private->params.get())
        m_private->params.get()->writeToDictionary(dict);
}

void WebCryptoKeyAlgorithm::assign(const WebCryptoKeyAlgorithm& other)
{
    m_private = other.m_private;
}

void WebCryptoKeyAlgorithm::reset()
{
    m_private.reset();
}

} // namespace blink
