// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/modules/v8/ScriptValueSerializerForModules.h"

#include "bindings/core/v8/SerializationTag.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/modules/v8/V8CryptoKey.h"
#include "bindings/modules/v8/V8DOMFileSystem.h"
#include "modules/filesystem/DOMFileSystem.h"
#include "public/platform/Platform.h"

namespace blink {
#ifndef MINIBLINK_NOT_IMPLEMENTED_WEBWORKER
enum CryptoKeyAlgorithmTag {
    AesCbcTag = 1,
    HmacTag = 2,
    RsaSsaPkcs1v1_5Tag = 3,
    // ID 4 was used by RsaEs, while still behind experimental flag.
    Sha1Tag = 5,
    Sha256Tag = 6,
    Sha384Tag = 7,
    Sha512Tag = 8,
    AesGcmTag = 9,
    RsaOaepTag = 10,
    AesCtrTag = 11,
    AesKwTag = 12,
    RsaPssTag = 13,
    EcdsaTag = 14,
    EcdhTag = 15,
    HkdfTag = 16,
    Pbkdf2Tag = 17,
    // Maximum allowed value is 2^32-1
};

enum NamedCurveTag {
    P256Tag = 1,
    P384Tag = 2,
    P521Tag = 3,
};

enum CryptoKeyUsage {
    // Extractability is not a "usage" in the WebCryptoKeyUsages sense, however
    // it fits conveniently into this bitfield.
    ExtractableUsage = 1 << 0,

    EncryptUsage = 1 << 1,
    DecryptUsage = 1 << 2,
    SignUsage = 1 << 3,
    VerifyUsage = 1 << 4,
    DeriveKeyUsage = 1 << 5,
    WrapKeyUsage = 1 << 6,
    UnwrapKeyUsage = 1 << 7,
    DeriveBitsUsage = 1 << 8,
    // Maximum allowed value is 1 << 31
};

enum CryptoKeySubTag {
    AesKeyTag = 1,
    HmacKeyTag = 2,
    // ID 3 was used by RsaKeyTag, while still behind experimental flag.
    RsaHashedKeyTag = 4,
    EcKeyTag = 5,
    NoParamsKeyTag = 6,
    // Maximum allowed value is 255
};

enum AssymetricCryptoKeyType {
    PublicKeyType = 1,
    PrivateKeyType = 2,
    // Maximum allowed value is 2^32-1
};

#endif // MINIBLINK_NOT_IMPLEMENTED

ScriptValueSerializerForModules::ScriptValueSerializerForModules(SerializedScriptValueWriter& writer, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray* blobInfo, BlobDataHandleMap& blobDataHandles, v8::TryCatch& tryCatch, ScriptState* scriptState)
    : ScriptValueSerializer(writer, messagePorts, arrayBuffers, blobInfo, blobDataHandles, tryCatch, scriptState)
{
}


ScriptValueSerializer::StateBase* ScriptValueSerializerForModules::writeDOMFileSystem(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    DOMFileSystem* fs = V8DOMFileSystem::toImpl(value.As<v8::Object>());
    if (!fs)
        return 0;
    if (!fs->clonable())
        return handleError(DataCloneError, "A FileSystem object could not be cloned.", next);

    toSerializedScriptValueWriterForModules(writer()).writeDOMFileSystem(fs->type(), fs->name(), fs->rootURL().string());
#endif
    return 0;
}

bool ScriptValueSerializerForModules::writeCryptoKey(v8::Local<v8::Value> value)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    CryptoKey* key = V8CryptoKey::toImpl(value.As<v8::Object>());
    if (!key)
        return false;
    return toSerializedScriptValueWriterForModules(writer()).writeCryptoKey(key->key());
#endif
    return false;
}

void SerializedScriptValueWriterForModules::writeDOMFileSystem(int type, const String& name, const String& url)
{
    append(DOMFileSystemTag);
    doWriteUint32(type);
    doWriteWebCoreString(name);
    doWriteWebCoreString(url);
}

bool SerializedScriptValueWriterForModules::writeCryptoKey(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    append(static_cast<uint8_t>(CryptoKeyTag));

    switch (key.algorithm().paramsType()) {
    case WebCryptoKeyAlgorithmParamsTypeAes:
        doWriteAesKey(key);
        break;
    case WebCryptoKeyAlgorithmParamsTypeHmac:
        doWriteHmacKey(key);
        break;
    case WebCryptoKeyAlgorithmParamsTypeRsaHashed:
        doWriteRsaHashedKey(key);
        break;
    case WebCryptoKeyAlgorithmParamsTypeEc:
        doWriteEcKey(key);
        break;
    case WebCryptoKeyAlgorithmParamsTypeNone:
        doWriteKeyWithoutParams(key);
        break;
    }

    doWriteKeyUsages(key.usages(), key.extractable());

    WebVector<uint8_t> keyData;
    if (!Platform::current()->crypto()->serializeKeyForClone(key, keyData))
        return false;

    doWriteUint32(keyData.size());
    append(keyData.data(), keyData.size());
    return true;
#endif
    return false;
}

void SerializedScriptValueWriterForModules::doWriteHmacKey(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    ASSERT(key.algorithm().paramsType() == WebCryptoKeyAlgorithmParamsTypeHmac);

    append(static_cast<uint8_t>(HmacKeyTag));
    ASSERT(!(key.algorithm().hmacParams()->lengthBits() % 8));
    doWriteUint32(key.algorithm().hmacParams()->lengthBits() / 8);
    doWriteAlgorithmId(key.algorithm().hmacParams()->hash().id());
#endif
}

void SerializedScriptValueWriterForModules::doWriteAesKey(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    ASSERT(key.algorithm().paramsType() == WebCryptoKeyAlgorithmParamsTypeAes);

    append(static_cast<uint8_t>(AesKeyTag));
    doWriteAlgorithmId(key.algorithm().id());
    // Converting the key length from bits to bytes is lossless and makes
    // it fit in 1 byte.
    ASSERT(!(key.algorithm().aesParams()->lengthBits() % 8));
    doWriteUint32(key.algorithm().aesParams()->lengthBits() / 8);
#endif
}

void SerializedScriptValueWriterForModules::doWriteRsaHashedKey(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    ASSERT(key.algorithm().rsaHashedParams());
    append(static_cast<uint8_t>(RsaHashedKeyTag));

    doWriteAlgorithmId(key.algorithm().id());
    doWriteAsymmetricKeyType(key.type());

    const WebCryptoRsaHashedKeyAlgorithmParams* params = key.algorithm().rsaHashedParams();
    doWriteUint32(params->modulusLengthBits());
    doWriteUint32(params->publicExponent().size());
    append(params->publicExponent().data(), params->publicExponent().size());
    doWriteAlgorithmId(params->hash().id());
#endif
}

void SerializedScriptValueWriterForModules::doWriteEcKey(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    ASSERT(key.algorithm().ecParams());
    append(static_cast<uint8_t>(EcKeyTag));

    doWriteAlgorithmId(key.algorithm().id());
    doWriteAsymmetricKeyType(key.type());
    doWriteNamedCurve(key.algorithm().ecParams()->namedCurve());
#endif
}

void SerializedScriptValueWriterForModules::doWriteKeyWithoutParams(const WebCryptoKey& key)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    ASSERT(WebCryptoAlgorithm::isKdf(key.algorithm().id()));
    append(static_cast<uint8_t>(NoParamsKeyTag));

    doWriteAlgorithmId(key.algorithm().id());
#endif
}

void SerializedScriptValueWriterForModules::doWriteAlgorithmId(WebCryptoAlgorithmId id)
{
    switch (id) {
    case WebCryptoAlgorithmIdAesCbc:
        return doWriteUint32(AesCbcTag);
    case WebCryptoAlgorithmIdHmac:
        return doWriteUint32(HmacTag);
    case WebCryptoAlgorithmIdRsaSsaPkcs1v1_5:
        return doWriteUint32(RsaSsaPkcs1v1_5Tag);
    case WebCryptoAlgorithmIdSha1:
        return doWriteUint32(Sha1Tag);
    case WebCryptoAlgorithmIdSha256:
        return doWriteUint32(Sha256Tag);
    case WebCryptoAlgorithmIdSha384:
        return doWriteUint32(Sha384Tag);
    case WebCryptoAlgorithmIdSha512:
        return doWriteUint32(Sha512Tag);
    case WebCryptoAlgorithmIdAesGcm:
        return doWriteUint32(AesGcmTag);
    case WebCryptoAlgorithmIdRsaOaep:
        return doWriteUint32(RsaOaepTag);
    case WebCryptoAlgorithmIdAesCtr:
        return doWriteUint32(AesCtrTag);
    case WebCryptoAlgorithmIdAesKw:
        return doWriteUint32(AesKwTag);
    case WebCryptoAlgorithmIdRsaPss:
        return doWriteUint32(RsaPssTag);
    case WebCryptoAlgorithmIdEcdsa:
        return doWriteUint32(EcdsaTag);
    case WebCryptoAlgorithmIdEcdh:
        return doWriteUint32(EcdhTag);
    case WebCryptoAlgorithmIdHkdf:
        return doWriteUint32(HkdfTag);
    case WebCryptoAlgorithmIdPbkdf2:
        return doWriteUint32(Pbkdf2Tag);
    }
    ASSERT_NOT_REACHED();
}

void SerializedScriptValueWriterForModules::doWriteAsymmetricKeyType(WebCryptoKeyType keyType)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    switch (keyType) {
    case WebCryptoKeyTypePublic:
        doWriteUint32(PublicKeyType);
        break;
    case WebCryptoKeyTypePrivate:
        doWriteUint32(PrivateKeyType);
        break;
    case WebCryptoKeyTypeSecret:
        ASSERT_NOT_REACHED();
    }
#endif
}

void SerializedScriptValueWriterForModules::doWriteNamedCurve(WebCryptoNamedCurve namedCurve)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    switch (namedCurve) {
    case WebCryptoNamedCurveP256:
        return doWriteUint32(P256Tag);
    case WebCryptoNamedCurveP384:
        return doWriteUint32(P384Tag);
    case WebCryptoNamedCurveP521:
        return doWriteUint32(P521Tag);
    }
    ASSERT_NOT_REACHED();
#endif
}

void SerializedScriptValueWriterForModules::doWriteKeyUsages(const WebCryptoKeyUsageMask usages, bool extractable)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED

    // Reminder to update this when adding new key usages.
    static_assert(EndOfWebCryptoKeyUsage == (1 << 7) + 1, "update required when adding new key usages");

    uint32_t value = 0;

    if (extractable)
        value |= ExtractableUsage;

    if (usages & WebCryptoKeyUsageEncrypt)
        value |= EncryptUsage;
    if (usages & WebCryptoKeyUsageDecrypt)
        value |= DecryptUsage;
    if (usages & WebCryptoKeyUsageSign)
        value |= SignUsage;
    if (usages & WebCryptoKeyUsageVerify)
        value |= VerifyUsage;
    if (usages & WebCryptoKeyUsageDeriveKey)
        value |= DeriveKeyUsage;
    if (usages & WebCryptoKeyUsageWrapKey)
        value |= WrapKeyUsage;
    if (usages & WebCryptoKeyUsageUnwrapKey)
        value |= UnwrapKeyUsage;
    if (usages & WebCryptoKeyUsageDeriveBits)
        value |= DeriveBitsUsage;

    doWriteUint32(value);
#endif
}

ScriptValueSerializer::StateBase* ScriptValueSerializerForModules::doSerializeValue(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    bool isDOMFileSystem = V8DOMFileSystem::hasInstance(value, isolate());
    if (isDOMFileSystem || V8CryptoKey::hasInstance(value, isolate())) {
        v8::Local<v8::Object> jsObject = value.As<v8::Object>();
        if (jsObject.IsEmpty())
            return handleError(DataCloneError, "An object could not be cloned.", next);
        greyObject(jsObject);

        if (isDOMFileSystem)
            return writeDOMFileSystem(value, next);

        if (!writeCryptoKey(value))
            return handleError(DataCloneError, "Couldn't serialize key data", next);

        return 0;
        //return handleError(DataCloneError, "Miniblink Couldn't serialize FileSystem or Crypto key data", next);
    }
#endif
    return ScriptValueSerializer::doSerializeValue(value, next);
}

#ifndef MINIBLINK_NOT_IMPLEMENTED_WEBWORKER

bool SerializedScriptValueReaderForModules::read(v8::Local<v8::Value>* value, ScriptValueCompositeCreator& creator)
{
    SerializationTag tag;
    if (!readTag(&tag))
        return false;
    switch (tag) {
    case DOMFileSystemTag:
        if (!readDOMFileSystem(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    case CryptoKeyTag:
        if (!readCryptoKey(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    default:
        return SerializedScriptValueReader::readWithTag(tag, value, creator);
    }
    return !value->IsEmpty();
}

bool SerializedScriptValueReaderForModules::readDOMFileSystem(v8::Local<v8::Value>* value)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    uint32_t type;
    String name;
    String url;
    if (!doReadUint32(&type))
        return false;
    if (!readWebCoreString(&name))
        return false;
    if (!readWebCoreString(&url))
        return false;
    DOMFileSystem* fs = DOMFileSystem::create(scriptState()->executionContext(), name, static_cast<FileSystemType>(type), KURL(ParsedURLString, url));
    *value = toV8(fs, scriptState()->context()->Global(), isolate());
    return !value->IsEmpty();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::readCryptoKey(v8::Local<v8::Value>* value)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    uint32_t rawKeyType;
    if (!doReadUint32(&rawKeyType))
        return false;

    WebCryptoKeyAlgorithm algorithm;
    WebCryptoKeyType type = WebCryptoKeyTypeSecret;

    switch (static_cast<CryptoKeySubTag>(rawKeyType)) {
    case AesKeyTag:
        if (!doReadAesKey(algorithm, type))
            return false;
        break;
    case HmacKeyTag:
        if (!doReadHmacKey(algorithm, type))
            return false;
        break;
    case RsaHashedKeyTag:
        if (!doReadRsaHashedKey(algorithm, type))
            return false;
        break;
    case EcKeyTag:
        if (!doReadEcKey(algorithm, type))
            return false;
        break;
    case NoParamsKeyTag:
        if (!doReadKeyWithoutParams(algorithm, type))
            return false;
        break;
    default:
        return false;
    }

    WebCryptoKeyUsageMask usages;
    bool extractable;
    if (!doReadKeyUsages(usages, extractable))
        return false;

    uint32_t keyDataLength;
    if (!doReadUint32(&keyDataLength))
        return false;

    if (position() + keyDataLength > length())
        return false;

    const uint8_t* keyData = allocate(keyDataLength);
    WebCryptoKey key = WebCryptoKey::createNull();
    if (!Platform::current()->crypto()->deserializeKeyForClone(
        algorithm, type, extractable, usages, keyData, keyDataLength, key)) {
        return false;
    }

    *value = toV8(CryptoKey::create(key), scriptState()->context()->Global(), isolate());
    return !value->IsEmpty();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadHmacKey(WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    uint32_t lengthBytes;
    if (!doReadUint32(&lengthBytes))
        return false;
    WebCryptoAlgorithmId hash;
    if (!doReadAlgorithmId(hash))
        return false;
    algorithm = WebCryptoKeyAlgorithm::createHmac(hash, lengthBytes * 8);
    type = WebCryptoKeyTypeSecret;
    return !algorithm.isNull();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadAesKey(WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebCryptoAlgorithmId id;
    if (!doReadAlgorithmId(id))
        return false;
    uint32_t lengthBytes;
    if (!doReadUint32(&lengthBytes))
        return false;
    algorithm = WebCryptoKeyAlgorithm::createAes(id, lengthBytes * 8);
    type = WebCryptoKeyTypeSecret;
    return !algorithm.isNull();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadRsaHashedKey(WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebCryptoAlgorithmId id;
    if (!doReadAlgorithmId(id))
        return false;

    if (!doReadAsymmetricKeyType(type))
        return false;

    uint32_t modulusLengthBits;
    if (!doReadUint32(&modulusLengthBits))
        return false;

    uint32_t publicExponentSize;
    if (!doReadUint32(&publicExponentSize))
        return false;

    if (position() + publicExponentSize > length())
        return false;

    const uint8_t* publicExponent = allocate(publicExponentSize);
    WebCryptoAlgorithmId hash;
    if (!doReadAlgorithmId(hash))
        return false;
    algorithm = WebCryptoKeyAlgorithm::createRsaHashed(id, modulusLengthBits, publicExponent, publicExponentSize, hash);

    return !algorithm.isNull();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadEcKey(WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebCryptoAlgorithmId id;
    if (!doReadAlgorithmId(id))
        return false;

    if (!doReadAsymmetricKeyType(type))
        return false;

    WebCryptoNamedCurve namedCurve;
    if (!doReadNamedCurve(namedCurve))
        return false;

    algorithm = WebCryptoKeyAlgorithm::createEc(id, namedCurve);
    return !algorithm.isNull();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadKeyWithoutParams(WebCryptoKeyAlgorithm& algorithm, WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebCryptoAlgorithmId id;
    if (!doReadAlgorithmId(id))
        return false;
    algorithm = WebCryptoKeyAlgorithm::createWithoutParams(id);
    type = WebCryptoKeyTypeSecret;
    return !algorithm.isNull();
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadAlgorithmId(WebCryptoAlgorithmId& id)
{
    uint32_t rawId;
    if (!doReadUint32(&rawId))
        return false;

    switch (static_cast<CryptoKeyAlgorithmTag>(rawId)) {
    case AesCbcTag:
        id = WebCryptoAlgorithmIdAesCbc;
        return true;
    case HmacTag:
        id = WebCryptoAlgorithmIdHmac;
        return true;
    case RsaSsaPkcs1v1_5Tag:
        id = WebCryptoAlgorithmIdRsaSsaPkcs1v1_5;
        return true;
    case Sha1Tag:
        id = WebCryptoAlgorithmIdSha1;
        return true;
    case Sha256Tag:
        id = WebCryptoAlgorithmIdSha256;
        return true;
    case Sha384Tag:
        id = WebCryptoAlgorithmIdSha384;
        return true;
    case Sha512Tag:
        id = WebCryptoAlgorithmIdSha512;
        return true;
    case AesGcmTag:
        id = WebCryptoAlgorithmIdAesGcm;
        return true;
    case RsaOaepTag:
        id = WebCryptoAlgorithmIdRsaOaep;
        return true;
    case AesCtrTag:
        id = WebCryptoAlgorithmIdAesCtr;
        return true;
    case AesKwTag:
        id = WebCryptoAlgorithmIdAesKw;
        return true;
    case RsaPssTag:
        id = WebCryptoAlgorithmIdRsaPss;
        return true;
    case EcdsaTag:
        id = WebCryptoAlgorithmIdEcdsa;
        return true;
    case EcdhTag:
        id = WebCryptoAlgorithmIdEcdh;
        return true;
    case HkdfTag:
        id = WebCryptoAlgorithmIdHkdf;
        return true;
    case Pbkdf2Tag:
        id = WebCryptoAlgorithmIdPbkdf2;
        return true;
    }

    return false;
}

bool SerializedScriptValueReaderForModules::doReadAsymmetricKeyType(WebCryptoKeyType& type)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    uint32_t rawType;
    if (!doReadUint32(&rawType))
        return false;

    switch (static_cast<AssymetricCryptoKeyType>(rawType)) {
    case PublicKeyType:
        type = WebCryptoKeyTypePublic;
        return true;
    case PrivateKeyType:
        type = WebCryptoKeyTypePrivate;
        return true;
    }
#endif

    return false;
}

bool SerializedScriptValueReaderForModules::doReadNamedCurve(WebCryptoNamedCurve& namedCurve)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    uint32_t rawName;
    if (!doReadUint32(&rawName))
        return false;

    switch (static_cast<NamedCurveTag>(rawName)) {
    case P256Tag:
        namedCurve = WebCryptoNamedCurveP256;
        return true;
    case P384Tag:
        namedCurve = WebCryptoNamedCurveP384;
        return true;
    case P521Tag:
        namedCurve = WebCryptoNamedCurveP521;
        return true;
    }
#endif
    return false;
}

bool SerializedScriptValueReaderForModules::doReadKeyUsages(WebCryptoKeyUsageMask& usages, bool& extractable)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // Reminder to update this when adding new key usages.
    static_assert(EndOfWebCryptoKeyUsage == (1 << 7) + 1, "update required when adding new key usages");
    const uint32_t allPossibleUsages = ExtractableUsage | EncryptUsage | DecryptUsage | SignUsage | VerifyUsage | DeriveKeyUsage | WrapKeyUsage | UnwrapKeyUsage | DeriveBitsUsage;

    uint32_t rawUsages;
    if (!doReadUint32(&rawUsages))
        return false;

    // Make sure it doesn't contain an unrecognized usage value.
    if (rawUsages & ~allPossibleUsages)
        return false;

    usages = 0;

    extractable = rawUsages & ExtractableUsage;

    if (rawUsages & EncryptUsage)
        usages |= WebCryptoKeyUsageEncrypt;
    if (rawUsages & DecryptUsage)
        usages |= WebCryptoKeyUsageDecrypt;
    if (rawUsages & SignUsage)
        usages |= WebCryptoKeyUsageSign;
    if (rawUsages & VerifyUsage)
        usages |= WebCryptoKeyUsageVerify;
    if (rawUsages & DeriveKeyUsage)
        usages |= WebCryptoKeyUsageDeriveKey;
    if (rawUsages & WrapKeyUsage)
        usages |= WebCryptoKeyUsageWrapKey;
    if (rawUsages & UnwrapKeyUsage)
        usages |= WebCryptoKeyUsageUnwrapKey;
    if (rawUsages & DeriveBitsUsage)
        usages |= WebCryptoKeyUsageDeriveBits;

    return true;
#endif
    return false;
}

ScriptValueDeserializerForModules::ScriptValueDeserializerForModules(SerializedScriptValueReaderForModules& reader, MessagePortArray* messagePorts, ArrayBufferContentsArray* arrayBufferContents)
    : ScriptValueDeserializer(reader, messagePorts, arrayBufferContents)
{
}

bool ScriptValueDeserializerForModules::read(v8::Local<v8::Value>* value)
{
    return toSerializedScriptValueReaderForModules(reader()).read(value, *this);
}
#endif // MINIBLINK_NOT_IMPLEMENTED
} // namespace blink
