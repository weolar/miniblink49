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
#include "modules/crypto/SubtleCrypto.h"

#include "bindings/core/v8/Dictionary.h"
#include "core/dom/ExecutionContext.h"
#include "modules/crypto/CryptoHistograms.h"
#include "modules/crypto/CryptoKey.h"
#include "modules/crypto/CryptoResultImpl.h"
#include "modules/crypto/NormalizeAlgorithm.h"
#include "platform/JSONValues.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCrypto.h"
#include "public/platform/WebCryptoAlgorithm.h"

namespace blink {

static bool parseAlgorithm(const AlgorithmIdentifier& raw, WebCryptoOperation op, WebCryptoAlgorithm& algorithm, CryptoResult* result)
{
    AlgorithmError error;
    bool success = normalizeAlgorithm(raw, op, algorithm, &error);
    if (!success)
        result->completeWithError(error.errorType, error.errorDetails);
    return success;
}

static bool canAccessWebCrypto(ScriptState* scriptState, CryptoResult* result)
{
    String errorMessage;
    if (!scriptState->executionContext()->isPrivilegedContext(errorMessage, ExecutionContext::WebCryptoPrivilegeCheck)) {
        result->completeWithError(WebCryptoErrorTypeNotSupported, errorMessage);
        return false;
    }

    return true;
}

static bool copyStringProperty(const char* property, const Dictionary& source, JSONObject* destination)
{
    String value;
    if (!DictionaryHelper::get(source, property, value))
        return false;
    destination->setString(property, value);
    return true;
}

static bool copySequenceOfStringProperty(const char* property, const Dictionary& source, JSONObject* destination)
{
    Vector<String> value;
    if (!DictionaryHelper::get(source, property, value))
        return false;
    RefPtr<JSONArray> jsonArray = JSONArray::create();
    for (unsigned i = 0; i < value.size(); ++i)
        jsonArray->pushString(value[i]);
    destination->setArray(property, jsonArray.release());
    return true;
}

// FIXME: At the time of writing this is not a part of the spec. It is based an
// an unpublished editor's draft for:
//   https://www.w3.org/Bugs/Public/show_bug.cgi?id=24963
// See http://crbug.com/373917.
static bool copyJwkDictionaryToJson(const Dictionary& dict, CString& jsonUtf8, CryptoResult* result)
{
    RefPtr<JSONObject> jsonObject = JSONObject::create();

    if (!copyStringProperty("kty", dict, jsonObject.get())) {
        result->completeWithError(WebCryptoErrorTypeData, "The required JWK member \"kty\" was missing");
        return false;
    }

    copyStringProperty("use", dict, jsonObject.get());
    copySequenceOfStringProperty("key_ops", dict, jsonObject.get());
    copyStringProperty("alg", dict, jsonObject.get());

    bool ext;
    if (DictionaryHelper::get(dict, "ext", ext))
        jsonObject->setBoolean("ext", ext);

    const char* const propertyNames[] = { "d", "n", "e", "p", "q", "dp", "dq", "qi", "k", "crv", "x", "y" };
    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(propertyNames); ++i)
        copyStringProperty(propertyNames[i], dict, jsonObject.get());

    String json = jsonObject->toJSONString();
    jsonUtf8 = json.utf8();
    return true;
}

SubtleCrypto::SubtleCrypto()
{
}

ScriptPromise SubtleCrypto::encrypt(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* key, const DOMArrayPiece& data)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationEncrypt, algorithm, result.get()))
        return promise;

    if (!key->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageEncrypt, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, key->key());
    Platform::current()->crypto()->encrypt(algorithm, key->key(), data.bytes(), data.byteLength(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::decrypt(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* key, const DOMArrayPiece& data)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationDecrypt, algorithm, result.get()))
        return promise;

    if (!key->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageDecrypt, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, key->key());
    Platform::current()->crypto()->decrypt(algorithm, key->key(), data.bytes(), data.byteLength(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::sign(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* key, const DOMArrayPiece& data)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationSign, algorithm, result.get()))
        return promise;

    if (!key->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageSign, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, key->key());
    Platform::current()->crypto()->sign(algorithm, key->key(), data.bytes(), data.byteLength(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::verifySignature(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* key, const DOMArrayPiece& signature, const DOMArrayPiece& data)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationVerify, algorithm, result.get()))
        return promise;

    if (!key->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageVerify, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, key->key());
    Platform::current()->crypto()->verifySignature(algorithm, key->key(), signature.bytes(), signature.byteLength(), data.bytes(), data.byteLength(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::digest(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, const DOMArrayPiece& data)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationDigest, algorithm, result.get()))
        return promise;

    histogramAlgorithm(scriptState->executionContext(), algorithm);
    Platform::current()->crypto()->digest(algorithm, data.bytes(), data.byteLength(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::generateKey(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, bool extractable, const Vector<String>& rawKeyUsages)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyUsageMask keyUsages;
    if (!CryptoKey::parseUsageMask(rawKeyUsages, keyUsages, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationGenerateKey, algorithm, result.get()))
        return promise;

    histogramAlgorithm(scriptState->executionContext(), algorithm);
    Platform::current()->crypto()->generateKey(algorithm, extractable, keyUsages, result->result());
    return promise;
}

ScriptPromise SubtleCrypto::importKey(ScriptState* scriptState, const String& rawFormat, const ArrayBufferOrArrayBufferViewOrDictionary& keyData, const AlgorithmIdentifier& rawAlgorithm, bool extractable, const Vector<String>& rawKeyUsages)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyFormat format;
    if (!CryptoKey::parseFormat(rawFormat, format, result.get()))
        return promise;

    if (keyData.isDictionary()) {
        if (format != WebCryptoKeyFormatJwk) {
            result->completeWithError(WebCryptoErrorTypeData, "Key data must be a buffer for non-JWK formats");
            return promise;
        }
    } else if (format == WebCryptoKeyFormatJwk) {
        result->completeWithError(WebCryptoErrorTypeData, "Key data must be an object for JWK import");
        return promise;
    }

    WebCryptoKeyUsageMask keyUsages;
    if (!CryptoKey::parseUsageMask(rawKeyUsages, keyUsages, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationImportKey, algorithm, result.get()))
        return promise;

    const unsigned char* ptr = nullptr;
    unsigned len = 0;

    CString jsonUtf8;
    if (keyData.isArrayBuffer()) {
        ptr = static_cast<const unsigned char*>(keyData.getAsArrayBuffer()->data());
        len = keyData.getAsArrayBuffer()->byteLength();
    } else if (keyData.isArrayBufferView()) {
        ptr = static_cast<const unsigned char*>(keyData.getAsArrayBufferView()->baseAddress());
        len = keyData.getAsArrayBufferView()->byteLength();
    } else if (keyData.isDictionary()) {
        if (!copyJwkDictionaryToJson(keyData.getAsDictionary(), jsonUtf8, result.get()))
            return promise;
        ptr = reinterpret_cast<const unsigned char*>(jsonUtf8.data());
        len = jsonUtf8.length();
    }
    histogramAlgorithm(scriptState->executionContext(), algorithm);
    Platform::current()->crypto()->importKey(format, ptr, len, algorithm, extractable, keyUsages, result->result());
    return promise;
}

ScriptPromise SubtleCrypto::exportKey(ScriptState* scriptState, const String& rawFormat, CryptoKey* key)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyFormat format;
    if (!CryptoKey::parseFormat(rawFormat, format, result.get()))
        return promise;

    if (!key->extractable()) {
        result->completeWithError(WebCryptoErrorTypeInvalidAccess, "key is not extractable");
        return promise;
    }

    histogramKey(scriptState->executionContext(), key->key());
    Platform::current()->crypto()->exportKey(format, key->key(), result->result());
    return promise;
}

ScriptPromise SubtleCrypto::wrapKey(ScriptState* scriptState, const String& rawFormat, CryptoKey* key, CryptoKey* wrappingKey, const AlgorithmIdentifier& rawWrapAlgorithm)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyFormat format;
    if (!CryptoKey::parseFormat(rawFormat, format, result.get()))
        return promise;

    WebCryptoAlgorithm wrapAlgorithm;
    if (!parseAlgorithm(rawWrapAlgorithm, WebCryptoOperationWrapKey, wrapAlgorithm, result.get()))
        return promise;

    if (!key->extractable()) {
        result->completeWithError(WebCryptoErrorTypeInvalidAccess, "key is not extractable");
        return promise;
    }

    if (!wrappingKey->canBeUsedForAlgorithm(wrapAlgorithm, WebCryptoKeyUsageWrapKey, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), wrapAlgorithm, wrappingKey->key());
    histogramKey(scriptState->executionContext(), key->key());
    Platform::current()->crypto()->wrapKey(format, key->key(), wrappingKey->key(), wrapAlgorithm, result->result());
    return promise;
}

ScriptPromise SubtleCrypto::unwrapKey(ScriptState* scriptState, const String& rawFormat, const DOMArrayPiece& wrappedKey, CryptoKey* unwrappingKey, const AlgorithmIdentifier& rawUnwrapAlgorithm, const AlgorithmIdentifier& rawUnwrappedKeyAlgorithm, bool extractable, const Vector<String>& rawKeyUsages)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyFormat format;
    if (!CryptoKey::parseFormat(rawFormat, format, result.get()))
        return promise;

    WebCryptoKeyUsageMask keyUsages;
    if (!CryptoKey::parseUsageMask(rawKeyUsages, keyUsages, result.get()))
        return promise;

    WebCryptoAlgorithm unwrapAlgorithm;
    if (!parseAlgorithm(rawUnwrapAlgorithm, WebCryptoOperationUnwrapKey, unwrapAlgorithm, result.get()))
        return promise;

    WebCryptoAlgorithm unwrappedKeyAlgorithm;
    if (!parseAlgorithm(rawUnwrappedKeyAlgorithm, WebCryptoOperationImportKey, unwrappedKeyAlgorithm, result.get()))
        return promise;

    if (!unwrappingKey->canBeUsedForAlgorithm(unwrapAlgorithm, WebCryptoKeyUsageUnwrapKey, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), unwrapAlgorithm, unwrappingKey->key());
    histogramAlgorithm(scriptState->executionContext(), unwrappedKeyAlgorithm);
    Platform::current()->crypto()->unwrapKey(format, wrappedKey.bytes(), wrappedKey.byteLength(), unwrappingKey->key(), unwrapAlgorithm, unwrappedKeyAlgorithm, extractable, keyUsages, result->result());
    return promise;
}

ScriptPromise SubtleCrypto::deriveBits(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* baseKey, unsigned lengthBits)
{
    RefPtrWillBeRawPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationDeriveBits, algorithm, result.get()))
        return promise;

    if (!baseKey->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageDeriveBits, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, baseKey->key());
    Platform::current()->crypto()->deriveBits(algorithm, baseKey->key(), lengthBits, result->result());
    return promise;
}

ScriptPromise SubtleCrypto::deriveKey(ScriptState* scriptState, const AlgorithmIdentifier& rawAlgorithm, CryptoKey* baseKey, const AlgorithmIdentifier& rawDerivedKeyType, bool extractable, const Vector<String>& rawKeyUsages)
{
    RefPtr<CryptoResultImpl> result = CryptoResultImpl::create(scriptState);
    ScriptPromise promise = result->promise();

    if (!canAccessWebCrypto(scriptState, result.get()))
        return promise;

    WebCryptoKeyUsageMask keyUsages;
    if (!CryptoKey::parseUsageMask(rawKeyUsages, keyUsages, result.get()))
        return promise;

    WebCryptoAlgorithm algorithm;
    if (!parseAlgorithm(rawAlgorithm, WebCryptoOperationDeriveBits, algorithm, result.get()))
        return promise;

    if (!baseKey->canBeUsedForAlgorithm(algorithm, WebCryptoKeyUsageDeriveKey, result.get()))
        return promise;

    WebCryptoAlgorithm importAlgorithm;
    if (!parseAlgorithm(rawDerivedKeyType, WebCryptoOperationImportKey, importAlgorithm, result.get()))
        return promise;

    WebCryptoAlgorithm keyLengthAlgorithm;
    if (!parseAlgorithm(rawDerivedKeyType, WebCryptoOperationGetKeyLength, keyLengthAlgorithm, result.get()))
        return promise;

    histogramAlgorithmAndKey(scriptState->executionContext(), algorithm, baseKey->key());
    histogramAlgorithm(scriptState->executionContext(), importAlgorithm);
    Platform::current()->crypto()->deriveKey(algorithm, baseKey->key(), importAlgorithm, keyLengthAlgorithm, extractable, keyUsages, result->result());
    return promise;
}

} // namespace blink
