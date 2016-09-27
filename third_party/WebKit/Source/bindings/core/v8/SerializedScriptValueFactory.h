// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SerializedScriptValueFactory_h
#define SerializedScriptValueFactory_h

#include "bindings/core/v8/ScriptValueSerializer.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/CoreExport.h"
#include "wtf/Noncopyable.h"

namespace blink {

class CORE_EXPORT SerializedScriptValueFactory {
    WTF_MAKE_NONCOPYABLE(SerializedScriptValueFactory);
public:
    SerializedScriptValueFactory() { }

    // If a serialization error occurs (e.g., cyclic input value) this
    // function returns an empty representation, schedules a V8 exception to
    // be thrown using v8::ThrowException(), and sets |didThrow|. In this case
    // the caller must not invoke any V8 operations until control returns to
    // V8. When serialization is successful, |didThrow| is false.
    virtual PassRefPtr<SerializedScriptValue> create(v8::Isolate*, v8::Local<v8::Value>, MessagePortArray*, ArrayBufferArray*, WebBlobInfoArray*, ExceptionState&);
    PassRefPtr<SerializedScriptValue> create(v8::Isolate*, v8::Local<v8::Value>, MessagePortArray*, ArrayBufferArray*, ExceptionState&);
    PassRefPtr<SerializedScriptValue> createFromWire(const String&);
    PassRefPtr<SerializedScriptValue> createFromWireBytes(const char* data, size_t length);
    PassRefPtr<SerializedScriptValue> create(const String&);
    virtual PassRefPtr<SerializedScriptValue> create(v8::Isolate*, const String&);
    PassRefPtr<SerializedScriptValue> create();
    PassRefPtr<SerializedScriptValue> create(v8::Isolate*, const ScriptValue&, WebBlobInfoArray*, ExceptionState&);

    // Never throws exceptions.
    PassRefPtr<SerializedScriptValue> createAndSwallowExceptions(v8::Isolate*, v8::Local<v8::Value>);

    v8::Local<v8::Value> deserialize(SerializedScriptValue*, v8::Isolate*, MessagePortArray*, const WebBlobInfoArray*);

    static SerializedScriptValueFactory& instance()
    {
        if (!m_instance) {
            ASSERT_NOT_REACHED();
            m_instance = new SerializedScriptValueFactory();
        }
        return *m_instance;
    }

    // SerializedScriptValueFactory::initialize() should be invoked when Blink is initialized,
    // i.e. initializeWithoutV8() in WebKit.cpp.
    static void initialize(SerializedScriptValueFactory* newFactory)
    {
        ASSERT(!m_instance);
        m_instance = newFactory;
    }

protected:
    ScriptValueSerializer::Status doSerialize(v8::Local<v8::Value>, SerializedScriptValueWriter&, MessagePortArray*, ArrayBufferArray*, WebBlobInfoArray*, SerializedScriptValue*, v8::TryCatch&, String& errorMessage, v8::Isolate*);
    virtual ScriptValueSerializer::Status doSerialize(v8::Local<v8::Value>, SerializedScriptValueWriter&, MessagePortArray*, ArrayBufferArray*, WebBlobInfoArray*, BlobDataHandleMap&, v8::TryCatch&, String& errorMessage, v8::Isolate*);
    void transferData(SerializedScriptValue*, SerializedScriptValueWriter&, ArrayBufferArray*, ExceptionState&, v8::Isolate*);

    virtual v8::Local<v8::Value> deserialize(String& data, BlobDataHandleMap& blobDataHandles, ArrayBufferContentsArray*, v8::Isolate*, MessagePortArray* messagePorts, const WebBlobInfoArray*);

private:
    static SerializedScriptValueFactory* m_instance;
};

} // namespace blink

#endif // SerializedScriptValueFactory_h
