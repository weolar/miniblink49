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
#include "modules/crypto/CryptoKey.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8ObjectBuilder.h"
#include "bindings/core/v8/V8Uint8Array.h"
#include "core/dom/ExceptionCode.h"
#include "platform/CryptoResult.h"
#include "public/platform/WebCryptoAlgorithmParams.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"
#include "public/platform/WebString.h"

namespace blink {

namespace {

const char* keyTypeToString(WebCryptoKeyType type)
{
    switch (type) {
    case WebCryptoKeyTypeSecret:
        return "secret";
    case WebCryptoKeyTypePublic:
        return "public";
    case WebCryptoKeyTypePrivate:
        return "private";
    }
    ASSERT_NOT_REACHED();
    return 0;
}

struct KeyUsageMapping {
    WebCryptoKeyUsage value;
    const char* const name;
};

// The order of this array is the same order that will appear in
// CryptoKey.usages. It must be kept ordered as described by the Web Crypto
// spec.
const KeyUsageMapping keyUsageMappings[] = {
    { WebCryptoKeyUsageEncrypt, "encrypt" },
    { WebCryptoKeyUsageDecrypt, "decrypt" },
    { WebCryptoKeyUsageSign, "sign" },
    { WebCryptoKeyUsageVerify, "verify" },
    { WebCryptoKeyUsageDeriveKey, "deriveKey" },
    { WebCryptoKeyUsageDeriveBits, "deriveBits" },
    { WebCryptoKeyUsageWrapKey, "wrapKey" },
    { WebCryptoKeyUsageUnwrapKey, "unwrapKey" },
};

static_assert(EndOfWebCryptoKeyUsage == (1 << 7) + 1, "keyUsageMappings needs to be updated");

const char* keyUsageToString(WebCryptoKeyUsage usage)
{
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(keyUsageMappings); ++i) {
        if (keyUsageMappings[i].value == usage)
            return keyUsageMappings[i].name;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

WebCryptoKeyUsageMask keyUsageStringToMask(const String& usageString)
{
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(keyUsageMappings); ++i) {
        if (keyUsageMappings[i].name == usageString)
            return keyUsageMappings[i].value;
    }
    return 0;
}

class DictionaryBuilder : public WebCryptoKeyAlgorithmDictionary {
public:
    explicit DictionaryBuilder(V8ObjectBuilder& builder)
        : m_builder(builder)
    {
    }

    virtual void setString(const char* propertyName, const char* value)
    {
        m_builder.addString(propertyName, value);
    }

    virtual void setUint(const char* propertyName, unsigned value)
    {
        m_builder.addNumber(propertyName, value);
    }

    virtual void setAlgorithm(const char* propertyName, const WebCryptoAlgorithm& algorithm)
    {
        ASSERT(algorithm.paramsType() == WebCryptoAlgorithmParamsTypeNone);

        V8ObjectBuilder algorithmValue(m_builder.scriptState());
        algorithmValue.addString("name", WebCryptoAlgorithm::lookupAlgorithmInfo(algorithm.id())->name);
        m_builder.add(propertyName, algorithmValue);
    }

    virtual void setUint8Array(const char* propertyName, const WebVector<unsigned char>& vector)
    {
        m_builder.add(propertyName, DOMUint8Array::create(vector.data(), vector.size()));
    }

private:
    V8ObjectBuilder& m_builder;
};

} // namespace

CryptoKey::~CryptoKey()
{
}

CryptoKey::CryptoKey(const WebCryptoKey& key)
    : m_key(key)
{
}

String CryptoKey::type() const
{
    return keyTypeToString(m_key.type());
}

bool CryptoKey::extractable() const
{
    return m_key.extractable();
}

ScriptValue CryptoKey::algorithm(ScriptState* scriptState)
{
    V8ObjectBuilder objectBuilder(scriptState);
    DictionaryBuilder dictionaryBuilder(objectBuilder);
    m_key.algorithm().writeToDictionary(&dictionaryBuilder);
    return objectBuilder.scriptValue();
}

// FIXME: This creates a new javascript array each time. What should happen
//        instead is return the same (immutable) array. (Javascript callers can
//        distinguish this by doing an == test on the arrays and seeing they are
//        different).
Vector<String> CryptoKey::usages() const
{
    Vector<String> result;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(keyUsageMappings); ++i) {
        WebCryptoKeyUsage usage = keyUsageMappings[i].value;
        if (m_key.usages() & usage)
            result.append(keyUsageToString(usage));
    }
    return result;
}

bool CryptoKey::canBeUsedForAlgorithm(const WebCryptoAlgorithm& algorithm, WebCryptoKeyUsage usage, CryptoResult* result) const
{
    if (!(m_key.usages() & usage)) {
        result->completeWithError(WebCryptoErrorTypeInvalidAccess, "key.usages does not permit this operation");
        return false;
    }

    if (m_key.algorithm().id() != algorithm.id()) {
        result->completeWithError(WebCryptoErrorTypeInvalidAccess, "key.algorithm does not match that of operation");
        return false;
    }

    return true;
}

bool CryptoKey::parseFormat(const String& formatString, WebCryptoKeyFormat& format, CryptoResult* result)
{
    // There are few enough values that testing serially is fast enough.
    if (formatString == "raw") {
        format = WebCryptoKeyFormatRaw;
        return true;
    }
    if (formatString == "pkcs8") {
        format = WebCryptoKeyFormatPkcs8;
        return true;
    }
    if (formatString == "spki") {
        format = WebCryptoKeyFormatSpki;
        return true;
    }
    if (formatString == "jwk") {
        format = WebCryptoKeyFormatJwk;
        return true;
    }

    result->completeWithError(WebCryptoErrorTypeType, "Invalid keyFormat argument");
    return false;
}

bool CryptoKey::parseUsageMask(const Vector<String>& usages, WebCryptoKeyUsageMask& mask, CryptoResult* result)
{
    mask = 0;
    for (size_t i = 0; i < usages.size(); ++i) {
        WebCryptoKeyUsageMask usage = keyUsageStringToMask(usages[i]);
        if (!usage) {
            result->completeWithError(WebCryptoErrorTypeType, "Invalid keyUsages argument");
            return false;
        }
        mask |= usage;
    }
    return true;
}

} // namespace blink
