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
#include "modules/crypto/NormalizeAlgorithm.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "bindings/modules/v8/V8CryptoKey.h"
#include "core/dom/DOMArrayPiece.h"
#include "core/dom/DOMTypedArray.h"
#include "public/platform/WebCryptoAlgorithmParams.h"
#include "public/platform/WebString.h"
#include "wtf/MathExtras.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuilder.h"
#include <algorithm>

namespace blink {

namespace {

typedef ArrayBufferOrArrayBufferView BufferSource;

struct AlgorithmNameMapping {
    // Must be an upper case ASCII string.
    const char* const algorithmName;
    // Must be strlen(algorithmName).
    unsigned char algorithmNameLength;
    WebCryptoAlgorithmId algorithmId;

#if ENABLE(ASSERT)
    bool operator<(const AlgorithmNameMapping&) const;
#endif
};

// Must be sorted by length, and then by reverse string.
// Also all names must be upper case ASCII.
const AlgorithmNameMapping algorithmNameMappings[] = {
    {"HMAC", 4, WebCryptoAlgorithmIdHmac},
    {"HKDF", 4, WebCryptoAlgorithmIdHkdf},
    {"ECDH", 4, WebCryptoAlgorithmIdEcdh},
    {"SHA-1", 5, WebCryptoAlgorithmIdSha1},
    {"ECDSA", 5, WebCryptoAlgorithmIdEcdsa},
    {"PBKDF2", 6, WebCryptoAlgorithmIdPbkdf2},
    {"AES-KW", 6, WebCryptoAlgorithmIdAesKw},
    {"SHA-512", 7, WebCryptoAlgorithmIdSha512},
    {"SHA-384", 7, WebCryptoAlgorithmIdSha384},
    {"SHA-256", 7, WebCryptoAlgorithmIdSha256},
    {"AES-CBC", 7, WebCryptoAlgorithmIdAesCbc},
    {"AES-GCM", 7, WebCryptoAlgorithmIdAesGcm},
    {"AES-CTR", 7, WebCryptoAlgorithmIdAesCtr},
    {"RSA-PSS", 7, WebCryptoAlgorithmIdRsaPss},
    {"RSA-OAEP", 8, WebCryptoAlgorithmIdRsaOaep},
    {"RSASSA-PKCS1-V1_5", 17, WebCryptoAlgorithmIdRsaSsaPkcs1v1_5},
};

// Reminder to update the table mapping names to IDs whenever adding a new
// algorithm ID.
static_assert(WebCryptoAlgorithmIdLast + 1 == WTF_ARRAY_LENGTH(algorithmNameMappings), "algorithmNameMappings needs to be updated");

#if ENABLE(ASSERT)

// Essentially std::is_sorted() (however that function is new to C++11).
template <typename Iterator>
bool isSorted(Iterator begin, Iterator end)
{
    if (begin == end)
        return true;

    Iterator prev = begin;
    Iterator cur = begin + 1;

    while (cur != end) {
        if (*cur < *prev)
            return false;
        cur++;
        prev++;
    }

    return true;
}

bool AlgorithmNameMapping::operator<(const AlgorithmNameMapping& o) const
{
    if (algorithmNameLength < o.algorithmNameLength)
        return true;
    if (algorithmNameLength > o.algorithmNameLength)
        return false;

    for (size_t i = 0; i < algorithmNameLength; ++i) {
        size_t reverseIndex = algorithmNameLength - i - 1;
        char c1 = algorithmName[reverseIndex];
        char c2 = o.algorithmName[reverseIndex];

        if (c1 < c2)
            return true;
        if (c1 > c2)
            return false;
    }

    return false;
}

bool verifyAlgorithmNameMappings(const AlgorithmNameMapping* begin, const AlgorithmNameMapping* end)
{
    for (const AlgorithmNameMapping* it = begin; it != end; ++it) {
        if (it->algorithmNameLength != strlen(it->algorithmName))
            return false;
        String str(it->algorithmName, it->algorithmNameLength);
        if (!str.containsOnlyASCII())
            return false;
        if (str.upper() != str)
            return false;
    }

    return isSorted(begin, end);
}
#endif

template <typename CharType>
bool algorithmNameComparator(const AlgorithmNameMapping& a, StringImpl* b)
{
    if (a.algorithmNameLength < b->length())
        return true;
    if (a.algorithmNameLength > b->length())
        return false;

    // Because the algorithm names contain many common prefixes, it is better
    // to compare starting at the end of the string.
    for (size_t i = 0; i < a.algorithmNameLength; ++i) {
        size_t reverseIndex = a.algorithmNameLength - i - 1;
        CharType c1 = a.algorithmName[reverseIndex];
        CharType c2 = b->getCharacters<CharType>()[reverseIndex];
        if (!isASCII(c2))
            return false;
        c2 = toASCIIUpper(c2);

        if (c1 < c2)
            return true;
        if (c1 > c2)
            return false;
    }

    return false;
}

bool lookupAlgorithmIdByName(const String& algorithmName, WebCryptoAlgorithmId& id)
{
    const AlgorithmNameMapping* begin = algorithmNameMappings;
    const AlgorithmNameMapping* end = algorithmNameMappings + WTF_ARRAY_LENGTH(algorithmNameMappings);

    ASSERT(verifyAlgorithmNameMappings(begin, end));

    const AlgorithmNameMapping* it;
    if (algorithmName.impl()->is8Bit())
        it = std::lower_bound(begin, end, algorithmName.impl(), &algorithmNameComparator<LChar>);
    else
        it = std::lower_bound(begin, end, algorithmName.impl(), &algorithmNameComparator<UChar>);

    if (it == end)
        return false;

    if (it->algorithmNameLength != algorithmName.length() || !equalIgnoringCase(algorithmName, it->algorithmName))
        return false;

    id = it->algorithmId;
    return true;
}

void setTypeError(const String& message, AlgorithmError* error)
{
    error->errorType = WebCryptoErrorTypeType;
    error->errorDetails = message;
}

void setNotSupportedError(const String& message, AlgorithmError* error)
{
    error->errorType = WebCryptoErrorTypeNotSupported;
    error->errorDetails = message;
}

// ErrorContext holds a stack of string literals which describe what was
// happening at the time the error occurred. This is helpful because
// parsing of the algorithm dictionary can be recursive and it is difficult to
// tell what went wrong from a failure alone.
class ErrorContext {
public:
    void add(const char* message)
    {
        m_messages.append(message);
    }

    void removeLast()
    {
        m_messages.removeLast();
    }

    // Join all of the string literals into a single String.
    String toString() const
    {
        if (m_messages.isEmpty())
            return String();

        StringBuilder result;
        const char* Separator = ": ";

        size_t length = (m_messages.size() - 1) * strlen(Separator);
        for (size_t i = 0; i < m_messages.size(); ++i)
            length += strlen(m_messages[i]);
        result.reserveCapacity(length);

        for (size_t i = 0; i < m_messages.size(); ++i) {
            if (i)
                result.append(Separator, strlen(Separator));
            result.append(m_messages[i], strlen(m_messages[i]));
        }

        return result.toString();
    }

    String toString(const char* message) const
    {
        ErrorContext stack(*this);
        stack.add(message);
        return stack.toString();
    }

    String toString(const char* message1, const char* message2) const
    {
        ErrorContext stack(*this);
        stack.add(message1);
        stack.add(message2);
        return stack.toString();
    }

private:
    // This inline size is large enough to avoid having to grow the Vector in
    // the majority of cases (up to 1 nested algorithm identifier).
    Vector<const char*, 10> m_messages;
};

// Defined by the WebCrypto spec as:
//
//     typedef (ArrayBuffer or ArrayBufferView) BufferSource;
//
bool getOptionalBufferSource(const Dictionary& raw, const char* propertyName, bool& hasProperty, BufferSource& buffer, const ErrorContext& context, AlgorithmError* error)
{
    hasProperty = false;
    v8::Local<v8::Value> v8Value;
    if (!raw.get(propertyName, v8Value))
        return true;
    hasProperty = true;

    if (v8Value->IsArrayBufferView()) {
        buffer.setArrayBufferView(V8ArrayBufferView::toImpl(v8::Local<v8::Object>::Cast(v8Value)));
        return true;
    }

    if (v8Value->IsArrayBuffer()) {
        buffer.setArrayBuffer(V8ArrayBuffer::toImpl(v8::Local<v8::Object>::Cast(v8Value)));
        return true;
    }

    if (hasProperty) {
        setTypeError(context.toString(propertyName, "Not a BufferSource"), error);
        return false;
    }
    return true;
}

bool getBufferSource(const Dictionary& raw, const char* propertyName, BufferSource& buffer, const ErrorContext& context, AlgorithmError* error)
{
    bool hasProperty;
    bool ok = getOptionalBufferSource(raw, propertyName, hasProperty, buffer, context, error);
    if (!hasProperty) {
        setTypeError(context.toString(propertyName, "Missing required property"), error);
        return false;
    }
    return ok;
}

bool getUint8Array(const Dictionary& raw, const char* propertyName, RefPtr<DOMUint8Array>& array, const ErrorContext& context, AlgorithmError* error)
{
    if (!DictionaryHelper::get(raw, propertyName, array) || !array) {
        setTypeError(context.toString(propertyName, "Missing or not a Uint8Array"), error);
        return false;
    }
    return true;
}

// Defined by the WebCrypto spec as:
//
//     typedef Uint8Array BigInteger;
bool getBigInteger(const Dictionary& raw, const char* propertyName, RefPtr<DOMUint8Array>& array, const ErrorContext& context, AlgorithmError* error)
{
    if (!getUint8Array(raw, propertyName, array, context, error))
        return false;

    if (!array->byteLength()) {
        // Empty BigIntegers represent 0 according to the spec
        array = DOMUint8Array::create(1);
    }

    return true;
}

// Gets an integer according to WebIDL's [EnforceRange].
bool getOptionalInteger(const Dictionary& raw, const char* propertyName, bool& hasProperty, double& value, double minValue, double maxValue, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    bool ok = DictionaryHelper::get(raw, propertyName, number, hasProperty);

    if (!hasProperty)
        return true;

    if (!ok || std::isnan(number)) {
        setTypeError(context.toString(propertyName, "Is not a number"), error);
        return false;
    }

    number = trunc(number);

    if (std::isinf(number) || number < minValue || number > maxValue) {
        setTypeError(context.toString(propertyName, "Outside of numeric range"), error);
        return false;
    }

    value = number;
    return true;
}

bool getInteger(const Dictionary& raw, const char* propertyName, double& value, double minValue, double maxValue, const ErrorContext& context, AlgorithmError* error)
{
    bool hasProperty;
    if (!getOptionalInteger(raw, propertyName, hasProperty, value, minValue, maxValue, context, error))
        return false;

    if (!hasProperty) {
        setTypeError(context.toString(propertyName, "Missing required property"), error);
        return false;
    }

    return true;
}

bool getUint32(const Dictionary& raw, const char* propertyName, uint32_t& value, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    if (!getInteger(raw, propertyName, number, 0, 0xFFFFFFFF, context, error))
        return false;
    value = number;
    return true;
}

bool getUint16(const Dictionary& raw, const char* propertyName, uint16_t& value, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    if (!getInteger(raw, propertyName, number, 0, 0xFFFF, context, error))
        return false;
    value = number;
    return true;
}

bool getUint8(const Dictionary& raw, const char* propertyName, uint8_t& value, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    if (!getInteger(raw, propertyName, number, 0, 0xFF, context, error))
        return false;
    value = number;
    return true;
}

bool getOptionalUint32(const Dictionary& raw, const char* propertyName, bool& hasValue, uint32_t& value, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    if (!getOptionalInteger(raw, propertyName, hasValue, number, 0, 0xFFFFFFFF, context, error))
        return false;
    if (hasValue)
        value = number;
    return true;
}

bool getOptionalUint8(const Dictionary& raw, const char* propertyName, bool& hasValue, uint8_t& value, const ErrorContext& context, AlgorithmError* error)
{
    double number;
    if (!getOptionalInteger(raw, propertyName, hasValue, number, 0, 0xFF, context, error))
        return false;
    if (hasValue)
        value = number;
    return true;
}

bool getAlgorithmIdentifier(const Dictionary& raw, const char* propertyName, AlgorithmIdentifier& value, const ErrorContext& context, AlgorithmError* error)
{
    // FIXME: This is not correct: http://crbug.com/438060
    //   (1) It may retrieve the property twice from the dictionary, whereas it
    //       should be reading the v8 value once to avoid issues with getters.
    //   (2) The value is stringified (whereas the spec says it should be an
    //       instance of DOMString).
    Dictionary dictionary;
    if (DictionaryHelper::get(raw, propertyName, dictionary) && !dictionary.isUndefinedOrNull()) {
        value.setDictionary(dictionary);
        return true;
    }

    String algorithmName;
    if (!DictionaryHelper::get(raw, propertyName, algorithmName)) {
        setTypeError(context.toString(propertyName, "Missing or not an AlgorithmIdentifier"), error);
        return false;
    }

    value.setString(algorithmName);
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary AesCbcParams : Algorithm {
//      required BufferSource iv;
//    };
bool parseAesCbcParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    BufferSource ivBufferSource;
    if (!getBufferSource(raw, "iv", ivBufferSource, context, error))
        return false;

    DOMArrayPiece iv(ivBufferSource);

    params = adoptPtr(new WebCryptoAesCbcParams(iv.bytes(), iv.byteLength()));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary AesKeyGenParams : Algorithm {
//      [EnforceRange] required unsigned short length;
//    };
bool parseAesKeyGenParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    uint16_t length;
    if (!getUint16(raw, "length", length, context, error))
        return false;

    params = adoptPtr(new WebCryptoAesKeyGenParams(length));
    return true;
}

bool parseAlgorithmIdentifier(const AlgorithmIdentifier&, WebCryptoOperation, WebCryptoAlgorithm&, ErrorContext, AlgorithmError*);

bool parseHash(const Dictionary& raw, WebCryptoAlgorithm& hash, ErrorContext context, AlgorithmError* error)
{
    AlgorithmIdentifier rawHash;
    if (!getAlgorithmIdentifier(raw, "hash", rawHash, context, error))
        return false;

    context.add("hash");
    return parseAlgorithmIdentifier(rawHash, WebCryptoOperationDigest, hash, context, error);
}

// Defined by the WebCrypto spec as:
//
//    dictionary HmacImportParams : Algorithm {
//      HashAlgorithmIdentifier hash;
//      [EnforceRange] unsigned long length;
//    };
//
// FIXME: http://crbug.com/438475: The current implementation differs from the
// spec in that the "hash" parameter is required. This seems more sensible, and
// is being proposed as a change to the spec. (https://www.w3.org/Bugs/Public/show_bug.cgi?id=27448).
bool parseHmacImportParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;

    bool hasLength;
    uint32_t length = 0;
    if (!getOptionalUint32(raw, "length", hasLength, length, context, error))
        return false;

    params = adoptPtr(new WebCryptoHmacImportParams(hash, hasLength, length));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary HmacKeyGenParams : Algorithm {
//      required HashAlgorithmIdentifier hash;
//      [EnforceRange] unsigned long length;
//    };
bool parseHmacKeyGenParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;

    bool hasLength;
    uint32_t length = 0;
    if (!getOptionalUint32(raw, "length", hasLength, length, context, error))
        return false;

    params = adoptPtr(new WebCryptoHmacKeyGenParams(hash, hasLength, length));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary RsaHashedImportParams : Algorithm {
//      required HashAlgorithmIdentifier hash;
//    };
bool parseRsaHashedImportParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;

    params = adoptPtr(new WebCryptoRsaHashedImportParams(hash));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary RsaKeyGenParams : Algorithm {
//      [EnforceRange] required unsigned long modulusLength;
//      required BigInteger publicExponent;
//    };
//
//    dictionary RsaHashedKeyGenParams : RsaKeyGenParams {
//      required HashAlgorithmIdentifier hash;
//    };
bool parseRsaHashedKeyGenParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    uint32_t modulusLength;
    if (!getUint32(raw, "modulusLength", modulusLength, context, error))
        return false;

    RefPtr<DOMUint8Array> publicExponent;
    if (!getBigInteger(raw, "publicExponent", publicExponent, context, error))
        return false;

    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;

    params = adoptPtr(new WebCryptoRsaHashedKeyGenParams(hash, modulusLength, static_cast<const unsigned char*>(publicExponent->baseAddress()), publicExponent->byteLength()));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary AesCtrParams : Algorithm {
//      required BufferSource counter;
//      [EnforceRange] required octet length;
//    };
bool parseAesCtrParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    BufferSource counterBufferSource;
    if (!getBufferSource(raw, "counter", counterBufferSource, context, error))
        return false;

    DOMArrayPiece counter(counterBufferSource);
    uint8_t length;
    if (!getUint8(raw, "length", length, context, error))
        return false;

    params = adoptPtr(new WebCryptoAesCtrParams(length, counter.bytes(), counter.byteLength()));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary AesGcmParams : Algorithm {
//       required BufferSource iv;
//       BufferSource additionalData;
//       [EnforceRange] octet tagLength;
//     }
bool parseAesGcmParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    BufferSource ivBufferSource;
    if (!getBufferSource(raw, "iv", ivBufferSource, context, error))
        return false;

    bool hasAdditionalData;
    BufferSource additionalDataBufferSource;
    if (!getOptionalBufferSource(raw, "additionalData", hasAdditionalData, additionalDataBufferSource, context, error))
        return false;

    uint8_t tagLength = 0;
    bool hasTagLength;
    if (!getOptionalUint8(raw, "tagLength", hasTagLength, tagLength, context, error))
        return false;

    DOMArrayPiece iv(ivBufferSource);
    DOMArrayPiece additionalData(additionalDataBufferSource, DOMArrayPiece::AllowNullPointToNullWithZeroSize);

    params = adoptPtr(new WebCryptoAesGcmParams(iv.bytes(), iv.byteLength(), hasAdditionalData, additionalData.bytes(), additionalData.byteLength(), hasTagLength, tagLength));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary RsaOaepParams : Algorithm {
//       BufferSource label;
//     };
bool parseRsaOaepParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    bool hasLabel;
    BufferSource labelBufferSource;
    if (!getOptionalBufferSource(raw, "label", hasLabel, labelBufferSource, context, error))
        return false;

    DOMArrayPiece label(labelBufferSource, DOMArrayPiece::AllowNullPointToNullWithZeroSize);
    params = adoptPtr(new WebCryptoRsaOaepParams(hasLabel, label.bytes(), label.byteLength()));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary RsaPssParams : Algorithm {
//       [EnforceRange] required unsigned long saltLength;
//     };
bool parseRsaPssParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    uint32_t saltLengthBytes;
    if (!getUint32(raw, "saltLength", saltLengthBytes, context, error))
        return false;

    params = adoptPtr(new WebCryptoRsaPssParams(saltLengthBytes));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary EcdsaParams : Algorithm {
//       required HashAlgorithmIdentifier hash;
//     };
bool parseEcdsaParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;

    params = adoptPtr(new WebCryptoEcdsaParams(hash));
    return true;
}

struct CurveNameMapping {
    const char* const name;
    WebCryptoNamedCurve value;
};

const CurveNameMapping curveNameMappings[] = {
    { "P-256", WebCryptoNamedCurveP256 },
    { "P-384", WebCryptoNamedCurveP384 },
    { "P-521", WebCryptoNamedCurveP521 }
};

// Reminder to update curveNameMappings when adding a new curve.
static_assert(WebCryptoNamedCurveLast + 1 == WTF_ARRAY_LENGTH(curveNameMappings), "curveNameMappings needs to be updated");

bool parseNamedCurve(const Dictionary& raw, WebCryptoNamedCurve& namedCurve, ErrorContext context, AlgorithmError* error)
{
    String namedCurveString;
    if (!DictionaryHelper::get(raw, "namedCurve", namedCurveString)) {
        setTypeError(context.toString("namedCurve", "Missing or not a string"), error);
        return false;
    }

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(curveNameMappings); ++i) {
        if (curveNameMappings[i].name == namedCurveString) {
            namedCurve = curveNameMappings[i].value;
            return true;
        }
    }

    setNotSupportedError(context.toString("Unrecognized namedCurve"), error);
    return false;
}

// Defined by the WebCrypto spec as:
//
//     dictionary EcKeyGenParams : Algorithm {
//       required NamedCurve namedCurve;
//     };
bool parseEcKeyGenParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoNamedCurve namedCurve;
    if (!parseNamedCurve(raw, namedCurve, context, error))
        return false;

    params = adoptPtr(new WebCryptoEcKeyGenParams(namedCurve));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary EcKeyImportParams : Algorithm {
//       required NamedCurve namedCurve;
//     };
bool parseEcKeyImportParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoNamedCurve namedCurve;
    if (!parseNamedCurve(raw, namedCurve, context, error))
        return false;

    params = adoptPtr(new WebCryptoEcKeyImportParams(namedCurve));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary EcdhKeyDeriveParams : Algorithm {
//       required CryptoKey public;
//     };
bool parseEcdhKeyDeriveParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    v8::Local<v8::Value> v8Value;
    if (!raw.get("public", v8Value)) {
        setTypeError(context.toString("public", "Missing required property"), error);
        return false;
    }

    CryptoKey* cryptoKey = V8CryptoKey::toImplWithTypeCheck(raw.isolate(), v8Value);
    if (!cryptoKey) {
        setTypeError(context.toString("public", "Must be a CryptoKey"), error);
        return false;
    }

    params = adoptPtr(new WebCryptoEcdhKeyDeriveParams(cryptoKey->key()));
    return true;
}

// Defined by the WebCrypto spec as:
//
//     dictionary Pbkdf2Params : Algorithm {
//       required BufferSource salt;
//       [EnforceRange] required unsigned long iterations;
//       required HashAlgorithmIdentifier hash;
//     };
bool parsePbkdf2Params(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    BufferSource saltBufferSource;
    if (!getBufferSource(raw, "salt", saltBufferSource, context, error))
        return false;

    DOMArrayPiece salt(saltBufferSource);

    uint32_t iterations;
    if (!getUint32(raw, "iterations", iterations, context, error))
        return false;

    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;
    params = adoptPtr(new WebCryptoPbkdf2Params(hash, salt.bytes(), salt.byteLength(), iterations));
    return true;
}

// Defined by the WebCrypto spec as:
//
//    dictionary AesDerivedKeyParams : Algorithm {
//      [EnforceRange] required unsigned short length;
//    };
bool parseAesDerivedKeyParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    uint16_t length;
    if (!getUint16(raw, "length", length, context, error))
        return false;

    params = adoptPtr(new WebCryptoAesDerivedKeyParams(length));
    return true;
}

// FIXME: once the spec has been updated, check that the implementation is
// still correct and update this comment. http://crbug.com/399095
//
// The WebCrypto spec hasn't been updated yet to define HKDF
// (https://www.w3.org/Bugs/Public/show_bug.cgi?id=27425). The assumed
// parameters are:
//
//    dictionary HkdfParams : Algorithm {
//      required HashAlgorithmIdentifier hash;
//      required BufferSource salt;
//      required BufferSource info;
//    };
bool parseHkdfParams(const Dictionary& raw, OwnPtr<WebCryptoAlgorithmParams>& params, const ErrorContext& context, AlgorithmError* error)
{
    WebCryptoAlgorithm hash;
    if (!parseHash(raw, hash, context, error))
        return false;
    BufferSource saltBufferSource;
    if (!getBufferSource(raw, "salt", saltBufferSource, context, error))
        return false;
    BufferSource infoBufferSource;
    if (!getBufferSource(raw, "info", infoBufferSource, context, error))
        return false;

    DOMArrayPiece salt(saltBufferSource);
    DOMArrayPiece info(infoBufferSource);

    params = adoptPtr(new WebCryptoHkdfParams(hash, salt.bytes(), salt.byteLength(), info.bytes(), info.byteLength()));
    return true;
}

bool parseAlgorithmParams(const Dictionary& raw, WebCryptoAlgorithmParamsType type, OwnPtr<WebCryptoAlgorithmParams>& params, ErrorContext& context, AlgorithmError* error)
{
    switch (type) {
    case WebCryptoAlgorithmParamsTypeNone:
        return true;
    case WebCryptoAlgorithmParamsTypeAesCbcParams:
        context.add("AesCbcParams");
        return parseAesCbcParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeAesKeyGenParams:
        context.add("AesKeyGenParams");
        return parseAesKeyGenParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeHmacImportParams:
        context.add("HmacImportParams");
        return parseHmacImportParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeHmacKeyGenParams:
        context.add("HmacKeyGenParams");
        return parseHmacKeyGenParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams:
        context.add("RsaHashedKeyGenParams");
        return parseRsaHashedKeyGenParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeRsaHashedImportParams:
        context.add("RsaHashedImportParams");
        return parseRsaHashedImportParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeAesCtrParams:
        context.add("AesCtrParams");
        return parseAesCtrParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeAesGcmParams:
        context.add("AesGcmParams");
        return parseAesGcmParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeRsaOaepParams:
        context.add("RsaOaepParams");
        return parseRsaOaepParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeRsaPssParams:
        context.add("RsaPssParams");
        return parseRsaPssParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeEcdsaParams:
        context.add("EcdsaParams");
        return parseEcdsaParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeEcKeyGenParams:
        context.add("EcKeyGenParams");
        return parseEcKeyGenParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeEcKeyImportParams:
        context.add("EcKeyImportParams");
        return parseEcKeyImportParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams:
        context.add("EcdhKeyDeriveParams");
        return parseEcdhKeyDeriveParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeAesDerivedKeyParams:
        context.add("AesDerivedKeyParams");
        return parseAesDerivedKeyParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypeHkdfParams:
        context.add("HkdfParams");
        return parseHkdfParams(raw, params, context, error);
    case WebCryptoAlgorithmParamsTypePbkdf2Params:
        context.add("Pbkdf2Params");
        return parsePbkdf2Params(raw, params, context, error);
    }
    ASSERT_NOT_REACHED();
    return false;
}

const char* operationToString(WebCryptoOperation op)
{
    switch (op) {
    case WebCryptoOperationEncrypt:
        return "encrypt";
    case WebCryptoOperationDecrypt:
        return "decrypt";
    case WebCryptoOperationSign:
        return "sign";
    case WebCryptoOperationVerify:
        return "verify";
    case WebCryptoOperationDigest:
        return "digest";
    case WebCryptoOperationGenerateKey:
        return "generateKey";
    case WebCryptoOperationImportKey:
        return "importKey";
    case WebCryptoOperationGetKeyLength:
        return "get key length";
    case WebCryptoOperationDeriveBits:
        return "deriveBits";
    case WebCryptoOperationWrapKey:
        return "wrapKey";
    case WebCryptoOperationUnwrapKey:
        return "unwrapKey";
    }
    return 0;
}

bool parseAlgorithmDictionary(const String& algorithmName, const Dictionary& raw, WebCryptoOperation op, WebCryptoAlgorithm& algorithm, ErrorContext context, AlgorithmError* error)
{
    WebCryptoAlgorithmId algorithmId;
    if (!lookupAlgorithmIdByName(algorithmName, algorithmId)) {
        setNotSupportedError(context.toString("Unrecognized name"), error);
        return false;
    }

    // Remove the "Algorithm:" prefix for all subsequent errors.
    context.removeLast();

    const WebCryptoAlgorithmInfo* algorithmInfo = WebCryptoAlgorithm::lookupAlgorithmInfo(algorithmId);

    if (algorithmInfo->operationToParamsType[op] == WebCryptoAlgorithmInfo::Undefined) {
        context.add(algorithmInfo->name);
        setNotSupportedError(context.toString("Unsupported operation", operationToString(op)), error);
        return false;
    }

    WebCryptoAlgorithmParamsType paramsType = static_cast<WebCryptoAlgorithmParamsType>(algorithmInfo->operationToParamsType[op]);

    OwnPtr<WebCryptoAlgorithmParams> params;
    if (!parseAlgorithmParams(raw, paramsType, params, context, error))
        return false;

    algorithm = WebCryptoAlgorithm(algorithmId, params.release());
    return true;
}

bool parseAlgorithmIdentifier(const AlgorithmIdentifier& raw, WebCryptoOperation op, WebCryptoAlgorithm& algorithm, ErrorContext context, AlgorithmError* error)
{
    context.add("Algorithm");

    // If the AlgorithmIdentifier is a String, treat it the same as a Dictionary with a "name" attribute and nothing else.
    if (raw.isString()) {
        return parseAlgorithmDictionary(raw.getAsString(), Dictionary(), op, algorithm, context, error);
    }

    Dictionary params = raw.getAsDictionary();

    // Get the name of the algorithm from the AlgorithmIdentifier.
    if (!params.isObject()) {
        setTypeError(context.toString("Not an object"), error);
        return false;
    }

    String algorithmName;
    if (!DictionaryHelper::get(params, "name", algorithmName)) {
        setTypeError(context.toString("name", "Missing or not a string"), error);
        return false;
    }

    return parseAlgorithmDictionary(algorithmName, params, op, algorithm, context, error);
}

} // namespace

bool normalizeAlgorithm(const AlgorithmIdentifier& raw, WebCryptoOperation op, WebCryptoAlgorithm& algorithm, AlgorithmError* error)
{
    return parseAlgorithmIdentifier(raw, op, algorithm, ErrorContext(), error);
}

} // namespace blink
