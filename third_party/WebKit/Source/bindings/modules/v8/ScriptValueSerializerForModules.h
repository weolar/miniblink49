// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScriptValueSerializerForModules_h
#define ScriptValueSerializerForModules_h

#include "bindings/core/v8/ScriptValueSerializer.h"
#include "public/platform/WebCrypto.h"
#include "public/platform/WebCryptoKey.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"
#include <v8.h>

namespace blink {

class SerializedScriptValueWriterForModules final : public SerializedScriptValueWriter {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(SerializedScriptValueWriterForModules);
public:
    SerializedScriptValueWriterForModules()
        : SerializedScriptValueWriter()
    {
    }

    void writeDOMFileSystem(int type, const String& name, const String& url);
    bool writeCryptoKey(const WebCryptoKey&);

private:
    void doWriteHmacKey(const WebCryptoKey&);
    void doWriteAesKey(const WebCryptoKey&);
    void doWriteRsaHashedKey(const WebCryptoKey&);
    void doWriteEcKey(const WebCryptoKey&);
    void doWriteKeyWithoutParams(const WebCryptoKey&);
    void doWriteAlgorithmId(WebCryptoAlgorithmId);
    void doWriteAsymmetricKeyType(WebCryptoKeyType);
    void doWriteNamedCurve(WebCryptoNamedCurve);
    void doWriteKeyUsages(const WebCryptoKeyUsageMask usages, bool extractable);
};

DEFINE_TYPE_CASTS(SerializedScriptValueWriterForModules, SerializedScriptValueWriter, writer, true, true);

class SerializedScriptValueReaderForModules final : public SerializedScriptValueReader {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(SerializedScriptValueReaderForModules);
public:
    SerializedScriptValueReaderForModules(const uint8_t* buffer, int length, const WebBlobInfoArray* blobInfo, BlobDataHandleMap& blobDataHandles, ScriptState* scriptState)
        : SerializedScriptValueReader(buffer, length, blobInfo, blobDataHandles, scriptState)
    {
    }

    bool read(v8::Local<v8::Value>*, ScriptValueCompositeCreator&) override;

private:
    bool readDOMFileSystem(v8::Local<v8::Value>*);
    bool readCryptoKey(v8::Local<v8::Value>*);
    bool doReadHmacKey(WebCryptoKeyAlgorithm&, WebCryptoKeyType&);
    bool doReadAesKey(WebCryptoKeyAlgorithm&, WebCryptoKeyType&);
    bool doReadRsaHashedKey(WebCryptoKeyAlgorithm&, WebCryptoKeyType&);
    bool doReadEcKey(WebCryptoKeyAlgorithm&, WebCryptoKeyType&);
    bool doReadKeyWithoutParams(WebCryptoKeyAlgorithm&, WebCryptoKeyType&);
    bool doReadAlgorithmId(WebCryptoAlgorithmId&);
    bool doReadAsymmetricKeyType(WebCryptoKeyType&);
    bool doReadNamedCurve(WebCryptoNamedCurve&);
    bool doReadKeyUsages(WebCryptoKeyUsageMask& usages, bool& extractable);
};

DEFINE_TYPE_CASTS(SerializedScriptValueReaderForModules, SerializedScriptValueReader, reader, true, true);

class ScriptValueSerializerForModules final : public ScriptValueSerializer {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(ScriptValueSerializerForModules);
public:
    ScriptValueSerializerForModules(SerializedScriptValueWriter&, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray*, BlobDataHandleMap& blobDataHandles, v8::TryCatch&, ScriptState*);

private:
    ScriptValueSerializer::StateBase* doSerializeValue(v8::Local<v8::Value>, ScriptValueSerializer::StateBase* next) override;

    ScriptValueSerializer::StateBase* writeDOMFileSystem(v8::Local<v8::Value>, ScriptValueSerializer::StateBase* next);
    bool writeCryptoKey(v8::Local<v8::Value>);
};

class ScriptValueDeserializerForModules final : public ScriptValueDeserializer {
    STACK_ALLOCATED();
    WTF_MAKE_NONCOPYABLE(ScriptValueDeserializerForModules);
public:
    ScriptValueDeserializerForModules(SerializedScriptValueReaderForModules&, MessagePortArray* messagePorts, ArrayBufferContentsArray*);

private:
    bool read(v8::Local<v8::Value>*) override;
};

} // namespace blink

#endif // ScriptValueSerializerForModules_h
