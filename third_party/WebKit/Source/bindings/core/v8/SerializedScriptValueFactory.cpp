// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptValueSerializer.h"
#include "wtf/ByteOrder.h"
#include "wtf/text/StringBuffer.h"

namespace blink {

SerializedScriptValueFactory* SerializedScriptValueFactory::m_instance = 0;

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create(v8::Isolate* isolate, v8::Local<v8::Value> value, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray* blobInfo, ExceptionState& exceptionState)
{
    RefPtr<SerializedScriptValue> serializedValue = create();
    SerializedScriptValueWriter writer;
    ScriptValueSerializer::Status status;
    String errorMessage;
    {
        v8::TryCatch tryCatch(isolate);
        status = doSerialize(value, writer, messagePorts, arrayBuffers, blobInfo, serializedValue.get(), tryCatch, errorMessage, isolate);
        if (status == ScriptValueSerializer::JSException) {
            // If there was a JS exception thrown, re-throw it.
            exceptionState.rethrowV8Exception(tryCatch.Exception());
            return serializedValue.release();
        }
    }
    switch (status) {
    case ScriptValueSerializer::InputError:
    case ScriptValueSerializer::DataCloneError:
        exceptionState.throwDOMException(ScriptValueSerializer::DataCloneError, errorMessage);
        return serializedValue.release();
    case ScriptValueSerializer::Success:
        transferData(serializedValue.get(), writer, arrayBuffers, exceptionState, isolate);
        return serializedValue.release();
    case ScriptValueSerializer::JSException:
        ASSERT_NOT_REACHED();
        break;
    }
    ASSERT_NOT_REACHED();
    return serializedValue.release();
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create(v8::Isolate* isolate, v8::Local<v8::Value> value, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, ExceptionState& exceptionState)
{
    return create(isolate, value, messagePorts, arrayBuffers, 0, exceptionState);
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::createAndSwallowExceptions(v8::Isolate* isolate, v8::Local<v8::Value> value)
{
    TrackExceptionState exceptionState;
    return create(isolate, value, 0, 0, exceptionState);
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create(v8::Isolate* isolate, const ScriptValue& value, WebBlobInfoArray* blobInfo, ExceptionState& exceptionState)
{
    ASSERT(isolate->InContext());
    return create(isolate, value.v8Value(), 0, 0, blobInfo, exceptionState);
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::createFromWire(const String& data)
{
    return adoptRef(new SerializedScriptValue(data));
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::createFromWireBytes(const char* data, size_t length)
{
    // Decode wire data from big endian to host byte order.
    ASSERT(!(length % sizeof(UChar)));
    size_t stringLength = length / sizeof(UChar);
    StringBuffer<UChar> buffer(stringLength);
    const UChar* src = reinterpret_cast<const UChar*>(data);
    UChar* dst = buffer.characters();
    for (size_t i = 0; i < stringLength; i++)
        dst[i] = ntohs(src[i]);

    return createFromWire(String::adopt(buffer));
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create(const String& data)
{
    return create(v8::Isolate::GetCurrent(), data);
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create(v8::Isolate* isolate, const String& data)
{
    ASSERT_NOT_REACHED();
    SerializedScriptValueWriter writer;
    writer.writeWebCoreString(data);
    String wireData = writer.takeWireString();
    return createFromWire(wireData);
}

PassRefPtr<SerializedScriptValue> SerializedScriptValueFactory::create()
{
    return adoptRef(new SerializedScriptValue());
}

void SerializedScriptValueFactory::transferData(SerializedScriptValue* serializedValue, SerializedScriptValueWriter& writer, ArrayBufferArray* arrayBuffers, ExceptionState& exceptionState, v8::Isolate* isolate)
{
    serializedValue->setData(writer.takeWireString());
    ASSERT(serializedValue->data().impl()->hasOneRef());
    if (!arrayBuffers || !arrayBuffers->size())
        return;
    serializedValue->transferArrayBuffers(isolate, *arrayBuffers, exceptionState);
}

ScriptValueSerializer::Status SerializedScriptValueFactory::doSerialize(v8::Local<v8::Value> value, SerializedScriptValueWriter& writer, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray* blobInfo, SerializedScriptValue* serializedValue, v8::TryCatch& tryCatch, String& errorMessage, v8::Isolate* isolate)
{
    return doSerialize(value, writer, messagePorts, arrayBuffers, blobInfo, serializedValue->blobDataHandles(), tryCatch, errorMessage, isolate);
}

ScriptValueSerializer::Status SerializedScriptValueFactory::doSerialize(v8::Local<v8::Value> value, SerializedScriptValueWriter& writer, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray* blobInfo, BlobDataHandleMap& blobDataHandles, v8::TryCatch& tryCatch, String& errorMessage, v8::Isolate* isolate)
{
    ScriptValueSerializer serializer(writer, messagePorts, arrayBuffers, blobInfo, blobDataHandles, tryCatch, ScriptState::current(isolate));
    ScriptValueSerializer::Status status = serializer.serialize(value);
    errorMessage = serializer.errorMessage();
    return status;
}

v8::Local<v8::Value> SerializedScriptValueFactory::deserialize(SerializedScriptValue* value, v8::Isolate* isolate, MessagePortArray* messagePorts, const WebBlobInfoArray* blobInfo)
{
    // deserialize() can run arbitrary script (e.g., setters), which could result in |this| being destroyed.
    // Holding a RefPtr ensures we are alive (along with our internal data) throughout the operation.
    RefPtr<SerializedScriptValue> protect(value);
    return deserialize(value->data(), value->blobDataHandles(), value->arrayBufferContentsArray(), isolate, messagePorts, blobInfo);
}

v8::Local<v8::Value> SerializedScriptValueFactory::deserialize(String& data, BlobDataHandleMap& blobDataHandles, ArrayBufferContentsArray* arrayBufferContentsArray, v8::Isolate* isolate, MessagePortArray* messagePorts, const WebBlobInfoArray* blobInfo)
{
    if (!data.impl())
        return v8::Null(isolate);
    static_assert(sizeof(SerializedScriptValueWriter::BufferValueType) == 2, "BufferValueType should be 2 bytes");
    data.ensure16Bit();
    // FIXME: SerializedScriptValue shouldn't use String for its underlying
    // storage. Instead, it should use SharedBuffer or Vector<uint8_t>. The
    // information stored in m_data isn't even encoded in UTF-16. Instead,
    // unicode characters are encoded as UTF-8 with two code units per UChar.
    SerializedScriptValueReader reader(reinterpret_cast<const uint8_t*>(data.impl()->characters16()), 2 * data.length(), blobInfo, blobDataHandles, ScriptState::current(isolate));
    ScriptValueDeserializer deserializer(reader, messagePorts, arrayBufferContentsArray);

    // deserialize() can run arbitrary script (e.g., setters), which could result in |this| being destroyed.
    // Holding a RefPtr ensures we are alive (along with our internal data) throughout the operation.
    return deserializer.deserialize();
}


} // namespace blink
