// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "bindings/core/v8/ScriptValueSerializer.h"
#include "config.h"

#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8CompositorProxy.h"
#include "bindings/core/v8/V8File.h"
#include "bindings/core/v8/V8FileList.h"
#include "bindings/core/v8/V8ImageData.h"
#include "bindings/core/v8/V8MessagePort.h"
#include "core/dom/CompositorProxy.h"
#include "core/dom/DOMDataView.h"
#include "core/fileapi/Blob.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileList.h"
#include "public/platform/Platform.h"
#include "public/platform/WebBlobInfo.h"
#include "wtf/DateMath.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/StringUTF8Adaptor.h"

// FIXME: consider crashing in debug mode on deserialization errors
// NOTE: be sure to change wireFormatVersion as necessary!

namespace blink {

// This code implements the HTML5 Structured Clone algorithm:
// http://www.whatwg.org/specs/web-apps/current-work/multipage/urls.html#safe-passing-of-structured-data

// ZigZag encoding helps VarInt encoding stay small for negative
// numbers with small absolute values.
class ZigZag {
public:
    static uint32_t encode(uint32_t value)
    {
        if (value & (1U << 31))
            value = ((~value) << 1) + 1;
        else
            value <<= 1;
        return value;
    }

    static uint32_t decode(uint32_t value)
    {
        if (value & 1)
            value = ~(value >> 1);
        else
            value >>= 1;
        return value;
    }

private:
    ZigZag();
};

static const int maxDepth = 20000;

static bool shouldCheckForCycles(int depth)
{
    ASSERT(depth >= 0);
    // Since we are not required to spot the cycle as soon as it
    // happens we can check for cycles only when the current depth
    // is a power of two.
    return !(depth & (depth - 1));
}

void SerializedScriptValueWriter::writeUndefined()
{
    append(UndefinedTag);
}

void SerializedScriptValueWriter::writeNull()
{
    append(NullTag);
}

void SerializedScriptValueWriter::writeTrue()
{
    append(TrueTag);
}

void SerializedScriptValueWriter::writeFalse()
{
    append(FalseTag);
}

void SerializedScriptValueWriter::writeBooleanObject(bool value)
{
    append(value ? TrueObjectTag : FalseObjectTag);
}

void SerializedScriptValueWriter::writeOneByteString(v8::Local<v8::String>& string)
{
    int stringLength = string->Length();
    int utf8Length = string->Utf8Length();
    ASSERT(stringLength >= 0 && utf8Length >= 0);

    append(StringTag);
    doWriteUint32(static_cast<uint32_t>(utf8Length));
    ensureSpace(utf8Length);

    // ASCII fast path.
    if (stringLength == utf8Length) {
        string->WriteOneByte(byteAt(m_position), 0, utf8Length, v8StringWriteOptions());
    } else {
        char* buffer = reinterpret_cast<char*>(byteAt(m_position));
        string->WriteUtf8(buffer, utf8Length, 0, v8StringWriteOptions());
    }
    m_position += utf8Length;
}

void SerializedScriptValueWriter::writeUCharString(v8::Local<v8::String>& string)
{
    int length = string->Length();
    ASSERT(length >= 0);

    int size = length * sizeof(UChar);
    int bytes = bytesNeededToWireEncode(static_cast<uint32_t>(size));
    if ((m_position + 1 + bytes) & 1)
        append(PaddingTag);

    append(StringUCharTag);
    doWriteUint32(static_cast<uint32_t>(size));
    ensureSpace(size);

    ASSERT(!(m_position & 1));
    uint16_t* buffer = reinterpret_cast<uint16_t*>(byteAt(m_position));
    string->Write(buffer, 0, length, v8StringWriteOptions());
    m_position += size;
}

void SerializedScriptValueWriter::writeStringObject(const char* data, int length)
{
    ASSERT(length >= 0);
    append(StringObjectTag);
    doWriteString(data, length);
}

void SerializedScriptValueWriter::writeWebCoreString(const String& string)
{
    // Uses UTF8 encoding so we can read it back as either V8 or
    // WebCore string.
    append(StringTag);
    doWriteWebCoreString(string);
}

void SerializedScriptValueWriter::writeVersion()
{
    append(VersionTag);
    doWriteUint32(SerializedScriptValue::wireFormatVersion);
}

void SerializedScriptValueWriter::writeInt32(int32_t value)
{
    append(Int32Tag);
    doWriteUint32(ZigZag::encode(static_cast<uint32_t>(value)));
}

void SerializedScriptValueWriter::writeUint32(uint32_t value)
{
    append(Uint32Tag);
    doWriteUint32(value);
}

void SerializedScriptValueWriter::writeDate(double numberValue)
{
    append(DateTag);
    doWriteNumber(numberValue);
}

void SerializedScriptValueWriter::writeNumber(double number)
{
    append(NumberTag);
    doWriteNumber(number);
}

void SerializedScriptValueWriter::writeNumberObject(double number)
{
    append(NumberObjectTag);
    doWriteNumber(number);
}

void SerializedScriptValueWriter::writeBlob(const String& uuid, const String& type, unsigned long long size)
{
    append(BlobTag);
    doWriteWebCoreString(uuid);
    doWriteWebCoreString(type);
    doWriteUint64(size);
}

void SerializedScriptValueWriter::writeBlobIndex(int blobIndex)
{
    ASSERT(blobIndex >= 0);
    append(BlobIndexTag);
    doWriteUint32(blobIndex);
}

void SerializedScriptValueWriter::writeCompositorProxy(const CompositorProxy& compositorProxy)
{
    append(CompositorProxyTag);
    doWriteUint64(compositorProxy.elementId());
    doWriteUint32(compositorProxy.bitfieldsSupported());
}

void SerializedScriptValueWriter::writeFile(const File& file)
{
    append(FileTag);
    doWriteFile(file);
}

void SerializedScriptValueWriter::writeFileIndex(int blobIndex)
{
    append(FileIndexTag);
    doWriteUint32(blobIndex);
}

void SerializedScriptValueWriter::writeFileList(const FileList& fileList)
{
    append(FileListTag);
    uint32_t length = fileList.length();
    doWriteUint32(length);
    for (unsigned i = 0; i < length; ++i)
        doWriteFile(*fileList.item(i));
}

void SerializedScriptValueWriter::writeFileListIndex(const Vector<int>& blobIndices)
{
    append(FileListIndexTag);
    uint32_t length = blobIndices.size();
    doWriteUint32(length);
    for (unsigned i = 0; i < length; ++i)
        doWriteUint32(blobIndices[i]);
}

void SerializedScriptValueWriter::writeArrayBuffer(const DOMArrayBuffer& arrayBuffer)
{
    append(ArrayBufferTag);
    doWriteArrayBuffer(arrayBuffer);
}

void SerializedScriptValueWriter::writeArrayBufferView(const DOMArrayBufferView& arrayBufferView)
{
    append(ArrayBufferViewTag);
#if ENABLE(ASSERT)
    const DOMArrayBuffer& arrayBuffer = *arrayBufferView.buffer();
    ASSERT(static_cast<const uint8_t*>(arrayBuffer.data()) + arrayBufferView.byteOffset() == static_cast<const uint8_t*>(arrayBufferView.baseAddress()));
#endif
    DOMArrayBufferView::ViewType type = arrayBufferView.type();

    switch (type) {
    case DOMArrayBufferView::TypeInt8:
        append(ByteArrayTag);
        break;
    case DOMArrayBufferView::TypeUint8Clamped:
        append(UnsignedByteClampedArrayTag);
        break;
    case DOMArrayBufferView::TypeUint8:
        append(UnsignedByteArrayTag);
        break;
    case DOMArrayBufferView::TypeInt16:
        append(ShortArrayTag);
        break;
    case DOMArrayBufferView::TypeUint16:
        append(UnsignedShortArrayTag);
        break;
    case DOMArrayBufferView::TypeInt32:
        append(IntArrayTag);
        break;
    case DOMArrayBufferView::TypeUint32:
        append(UnsignedIntArrayTag);
        break;
    case DOMArrayBufferView::TypeFloat32:
        append(FloatArrayTag);
        break;
    case DOMArrayBufferView::TypeFloat64:
        append(DoubleArrayTag);
        break;
    case DOMArrayBufferView::TypeDataView:
        append(DataViewTag);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    doWriteUint32(arrayBufferView.byteOffset());
    doWriteUint32(arrayBufferView.byteLength());
}

void SerializedScriptValueWriter::writeImageData(uint32_t width, uint32_t height, const uint8_t* pixelData, uint32_t pixelDataLength)
{
    append(ImageDataTag);
    doWriteUint32(width);
    doWriteUint32(height);
    doWriteUint32(pixelDataLength);
    append(pixelData, pixelDataLength);
}

void SerializedScriptValueWriter::writeRegExp(v8::Local<v8::String> pattern, v8::RegExp::Flags flags)
{
    append(RegExpTag);
    v8::String::Utf8Value patternUtf8Value(pattern);
    doWriteString(*patternUtf8Value, patternUtf8Value.length());
    doWriteUint32(static_cast<uint32_t>(flags));
}

void SerializedScriptValueWriter::writeTransferredMessagePort(uint32_t index)
{
    append(MessagePortTag);
    doWriteUint32(index);
}

void SerializedScriptValueWriter::writeTransferredArrayBuffer(uint32_t index)
{
    append(ArrayBufferTransferTag);
    doWriteUint32(index);
}

void SerializedScriptValueWriter::writeObjectReference(uint32_t reference)
{
    append(ObjectReferenceTag);
    doWriteUint32(reference);
}

void SerializedScriptValueWriter::writeObject(uint32_t numProperties)
{
    append(ObjectTag);
    doWriteUint32(numProperties);
}

void SerializedScriptValueWriter::writeSparseArray(uint32_t numProperties, uint32_t length)
{
    append(SparseArrayTag);
    doWriteUint32(numProperties);
    doWriteUint32(length);
}

void SerializedScriptValueWriter::writeDenseArray(uint32_t numProperties, uint32_t length)
{
    append(DenseArrayTag);
    doWriteUint32(numProperties);
    doWriteUint32(length);
}

String SerializedScriptValueWriter::takeWireString()
{
    static_assert(sizeof(BufferValueType) == 2, "BufferValueType should be 2 bytes");
    fillHole();
    String data = String(m_buffer.data(), m_buffer.size());
    data.impl()->truncateAssumingIsolated((m_position + 1) / sizeof(BufferValueType));
    return data;
}

void SerializedScriptValueWriter::writeReferenceCount(uint32_t numberOfReferences)
{
    append(ReferenceCountTag);
    doWriteUint32(numberOfReferences);
}

void SerializedScriptValueWriter::writeGenerateFreshObject()
{
    append(GenerateFreshObjectTag);
}

void SerializedScriptValueWriter::writeGenerateFreshSparseArray(uint32_t length)
{
    append(GenerateFreshSparseArrayTag);
    doWriteUint32(length);
}

void SerializedScriptValueWriter::writeGenerateFreshDenseArray(uint32_t length)
{
    append(GenerateFreshDenseArrayTag);
    doWriteUint32(length);
}

void SerializedScriptValueWriter::writeGenerateFreshMap()
{
    append(GenerateFreshMapTag);
}

void SerializedScriptValueWriter::writeGenerateFreshSet()
{
    append(GenerateFreshSetTag);
}

void SerializedScriptValueWriter::writeMap(uint32_t length)
{
    append(MapTag);
    doWriteUint32(length);
}

void SerializedScriptValueWriter::writeSet(uint32_t length)
{
    append(SetTag);
    doWriteUint32(length);
}

void SerializedScriptValueWriter::doWriteFile(const File& file)
{
    doWriteWebCoreString(file.hasBackingFile() ? file.path() : "");
    doWriteWebCoreString(file.name());
    doWriteWebCoreString(file.webkitRelativePath());
    doWriteWebCoreString(file.uuid());
    doWriteWebCoreString(file.type());

    // FIXME don't use 1 byte to encode a flag.
    if (file.hasValidSnapshotMetadata()) {
        doWriteUint32(static_cast<uint8_t>(1));

        long long size;
        double lastModifiedMS;
        file.captureSnapshot(size, lastModifiedMS);
        doWriteUint64(static_cast<uint64_t>(size));
        doWriteNumber(lastModifiedMS);
    } else {
        doWriteUint32(static_cast<uint8_t>(0));
    }

    doWriteUint32(static_cast<uint8_t>((file.userVisibility() == File::IsUserVisible) ? 1 : 0));
}

void SerializedScriptValueWriter::doWriteArrayBuffer(const DOMArrayBuffer& arrayBuffer)
{
    uint32_t byteLength = arrayBuffer.byteLength();
    doWriteUint32(byteLength);
    append(static_cast<const uint8_t*>(arrayBuffer.data()), byteLength);
}

void SerializedScriptValueWriter::doWriteString(const char* data, int length)
{
    doWriteUint32(static_cast<uint32_t>(length));
    append(reinterpret_cast<const uint8_t*>(data), length);
}

void SerializedScriptValueWriter::doWriteWebCoreString(const String& string)
{
    StringUTF8Adaptor stringUTF8(string);
    doWriteString(stringUTF8.data(), stringUTF8.length());
}

int SerializedScriptValueWriter::bytesNeededToWireEncode(uint32_t value)
{
    int bytes = 1;
    while (true) {
        value >>= SerializedScriptValue::varIntShift;
        if (!value)
            break;
        ++bytes;
    }

    return bytes;
}

void SerializedScriptValueWriter::doWriteUint32(uint32_t value)
{
    doWriteUintHelper(value);
}

void SerializedScriptValueWriter::doWriteUint64(uint64_t value)
{
    doWriteUintHelper(value);
}

void SerializedScriptValueWriter::doWriteNumber(double number)
{
    append(reinterpret_cast<uint8_t*>(&number), sizeof(number));
}

void SerializedScriptValueWriter::append(SerializationTag tag)
{
    append(static_cast<uint8_t>(tag));
}

void SerializedScriptValueWriter::append(uint8_t b)
{
    ensureSpace(1);
    *byteAt(m_position++) = b;
}

void SerializedScriptValueWriter::append(const uint8_t* data, int length)
{
    ensureSpace(length);
    memcpy(byteAt(m_position), data, length);
    m_position += length;
}

void SerializedScriptValueWriter::ensureSpace(unsigned extra)
{
    static_assert(sizeof(BufferValueType) == 2, "BufferValueType should be 2 bytes");
    m_buffer.resize((m_position + extra + 1) / sizeof(BufferValueType)); // "+ 1" to round up.
}

void SerializedScriptValueWriter::fillHole()
{
    static_assert(sizeof(BufferValueType) == 2, "BufferValueType should be 2 bytes");
    // If the writer is at odd position in the buffer, then one of
    // the bytes in the last UChar is not initialized.
    if (m_position % 2)
        *byteAt(m_position) = static_cast<uint8_t>(PaddingTag);
}

uint8_t* SerializedScriptValueWriter::byteAt(int position)
{
    return reinterpret_cast<uint8_t*>(m_buffer.data()) + position;
}

int SerializedScriptValueWriter::v8StringWriteOptions()
{
    return v8::String::NO_NULL_TERMINATION;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::AbstractObjectState::serializeProperties(bool ignoreIndexed, ScriptValueSerializer& serializer)
{
    while (m_index < m_propertyNames->Length()) {
        if (!m_nameDone) {
            v8::Local<v8::Value> propertyName;
            if (!m_propertyNames->Get(serializer.context(), m_index).ToLocal(&propertyName))
                return serializer.handleError(JSException, "Failed to get a property while cloning an object.", this);
            bool hasStringProperty = propertyName->IsString() && v8CallBoolean(composite()->HasRealNamedProperty(serializer.context(), propertyName.As<v8::String>()));
            if (StateBase* newState = serializer.checkException(this))
                return newState;
            bool hasIndexedProperty = !hasStringProperty && propertyName->IsUint32() && v8CallBoolean(composite()->HasRealIndexedProperty(serializer.context(), propertyName.As<v8::Uint32>()->Value()));
            if (StateBase* newState = serializer.checkException(this))
                return newState;
            if (hasStringProperty || (hasIndexedProperty && !ignoreIndexed)) {
                m_propertyName = propertyName;
            } else {
                ++m_index;
                continue;
            }
        }
        ASSERT(!m_propertyName.IsEmpty());
        if (!m_nameDone) {
            m_nameDone = true;
            if (StateBase* newState = serializer.doSerialize(m_propertyName, this))
                return newState;
        }
        v8::Local<v8::Value> value;
        if (!composite()->Get(serializer.context(), m_propertyName).ToLocal(&value))
            return serializer.handleError(JSException, "Failed to get a property while cloning an object.", this);
        m_nameDone = false;
        m_propertyName.Clear();
        ++m_index;
        ++m_numSerializedProperties;
        // If we return early here, it's either because we have pushed a new state onto the
        // serialization state stack or because we have encountered an error (and in both cases
        // we are unwinding the native stack).
        if (StateBase* newState = serializer.doSerialize(value, this))
            return newState;
    }
    return objectDone(m_numSerializedProperties, serializer);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::ObjectState::advance(ScriptValueSerializer& serializer)
{
    if (m_propertyNames.IsEmpty()) {
        if (!composite()->GetPropertyNames(serializer.context()).ToLocal(&m_propertyNames))
            return serializer.checkException(this);
    }
    return serializeProperties(false, serializer);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::ObjectState::objectDone(unsigned numProperties, ScriptValueSerializer& serializer)
{
    return serializer.writeObject(numProperties, this);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::DenseArrayState::advance(ScriptValueSerializer& serializer)
{
    while (m_arrayIndex < m_arrayLength) {
        v8::Local<v8::Value> value;
        if (!composite().As<v8::Array>()->Get(serializer.context(), m_arrayIndex).ToLocal(&value))
            return serializer.handleError(JSException, "Failed to get an element while cloning an array.", this);
        m_arrayIndex++;
        if (StateBase* newState = serializer.checkException(this))
            return newState;
        if (StateBase* newState = serializer.doSerialize(value, this))
            return newState;
    }
    return serializeProperties(true, serializer);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::DenseArrayState::objectDone(unsigned numProperties, ScriptValueSerializer& serializer)
{
    return serializer.writeDenseArray(numProperties, m_arrayLength, this);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::SparseArrayState::advance(ScriptValueSerializer& serializer)
{
    return serializeProperties(false, serializer);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::SparseArrayState::objectDone(unsigned numProperties, ScriptValueSerializer& serializer)
{
    return serializer.writeSparseArray(numProperties, composite().As<v8::Array>()->Length(), this);
}

template <typename T>
ScriptValueSerializer::StateBase* ScriptValueSerializer::CollectionState<T>::advance(ScriptValueSerializer& serializer)
{
    while (m_index < m_length) {
        v8::Local<v8::Value> value;
        if (!m_entries->Get(serializer.context(), m_index).ToLocal(&value))
            return serializer.handleError(JSException, "Failed to get an element while cloning a collection.", this);
        m_index++;
        if (StateBase* newState = serializer.checkException(this))
            return newState;
        if (StateBase* newState = serializer.doSerialize(value, this))
            return newState;
    }
    return serializer.writeCollection<T>(m_length, this);
}

static v8::Local<v8::Object> toV8Object(MessagePort* impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!impl)
        return v8::Local<v8::Object>();
    v8::Local<v8::Value> wrapper = toV8(impl, creationContext, isolate);
    if (wrapper.IsEmpty())
        return v8::Local<v8::Object>();
    ASSERT(wrapper->IsObject());
    return wrapper.As<v8::Object>();
}

static v8::Local<v8::ArrayBuffer> toV8Object(DOMArrayBuffer* impl, v8::Local<v8::Object> creationContext, v8::Isolate* isolate)
{
    if (!impl)
        return v8::Local<v8::ArrayBuffer>();
    v8::Local<v8::Value> wrapper = toV8(impl, creationContext, isolate);
    if (wrapper.IsEmpty())
        return v8::Local<v8::ArrayBuffer>();
    ASSERT(wrapper->IsArrayBuffer());
    return wrapper.As<v8::ArrayBuffer>();
}

// Returns true if the provided object is to be considered a 'host object', as used in the
// HTML5 structured clone algorithm.
static bool isHostObject(v8::Local<v8::Object> object)
{
    // If the object has any internal fields, then we won't be able to serialize or deserialize
    // them; conveniently, this is also a quick way to detect DOM wrapper objects, because
    // the mechanism for these relies on data stored in these fields. We should
    // catch external array data as a special case.
    return object->InternalFieldCount();
}

ScriptValueSerializer::ScriptValueSerializer(SerializedScriptValueWriter& writer, MessagePortArray* messagePorts, ArrayBufferArray* arrayBuffers, WebBlobInfoArray* blobInfo, BlobDataHandleMap& blobDataHandles, v8::TryCatch& tryCatch, ScriptState* scriptState)
    : m_scriptState(scriptState)
    , m_writer(writer)
    , m_tryCatch(tryCatch)
    , m_depth(0)
    , m_status(Success)
    , m_nextObjectReference(0)
    , m_blobInfo(blobInfo)
    , m_blobDataHandles(blobDataHandles)
{
    ASSERT(!tryCatch.HasCaught());
    v8::Local<v8::Object> creationContext = m_scriptState->context()->Global();
    if (messagePorts) {
        for (size_t i = 0; i < messagePorts->size(); i++)
            m_transferredMessagePorts.set(toV8Object(messagePorts->at(i).get(), creationContext, isolate()), i);
    }
    if (arrayBuffers) {
        for (size_t i = 0; i < arrayBuffers->size(); i++) {
            v8::Local<v8::Object> v8ArrayBuffer = toV8Object(arrayBuffers->at(i).get(), creationContext, isolate());
            // Coalesce multiple occurences of the same buffer to the first index.
            if (!m_transferredArrayBuffers.contains(v8ArrayBuffer))
                m_transferredArrayBuffers.set(v8ArrayBuffer, i);
        }
    }
}

ScriptValueSerializer::Status ScriptValueSerializer::serialize(v8::Local<v8::Value> value)
{
    v8::HandleScope scope(isolate());
    m_writer.writeVersion();
    StateBase* state = doSerialize(value, 0);
    while (state)
        state = state->advance(*this);
    return m_status;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::doSerialize(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    m_writer.writeReferenceCount(m_nextObjectReference);
    uint32_t objectReference;
    if ((value->IsObject() || value->IsDate() || value->IsRegExp())
        && m_objectPool.tryGet(value.As<v8::Object>(), &objectReference)) {
        // Note that IsObject() also detects wrappers (eg, it will catch the things
        // that we grey and write below).
        ASSERT(!value->IsString());
        m_writer.writeObjectReference(objectReference);
    } else {
        return doSerializeValue(value, next);
    }
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::doSerializeValue(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    uint32_t arrayBufferIndex;
    if (value.IsEmpty())
        return handleError(InputError, "The empty property name cannot be cloned.", next);
    if (value->IsUndefined()) {
        m_writer.writeUndefined();
    } else if (value->IsNull()) {
        m_writer.writeNull();
    } else if (value->IsTrue()) {
        m_writer.writeTrue();
    } else if (value->IsFalse()) {
        m_writer.writeFalse();
    } else if (value->IsInt32()) {
        m_writer.writeInt32(value.As<v8::Int32>()->Value());
    } else if (value->IsUint32()) {
        m_writer.writeUint32(value.As<v8::Uint32>()->Value());
    } else if (value->IsNumber()) {
        m_writer.writeNumber(value.As<v8::Number>()->Value());
    } else if (V8ArrayBufferView::hasInstance(value, isolate())) {
        return writeAndGreyArrayBufferView(value.As<v8::Object>(), next);
    } else if (value->IsString()) {
        writeString(value);
    } else if (V8MessagePort::hasInstance(value, isolate())) {
        uint32_t messagePortIndex;
        if (m_transferredMessagePorts.tryGet(value.As<v8::Object>(), &messagePortIndex)) {
            m_writer.writeTransferredMessagePort(messagePortIndex);
        } else {
            return handleError(DataCloneError, "A MessagePort could not be cloned.", next);
        }
    } else if (V8ArrayBuffer::hasInstance(value, isolate()) && m_transferredArrayBuffers.tryGet(value.As<v8::Object>(), &arrayBufferIndex)) {
        return writeTransferredArrayBuffer(value, arrayBufferIndex, next);
    } else {
        v8::Local<v8::Object> jsObject = value.As<v8::Object>();
        if (jsObject.IsEmpty())
            return handleError(DataCloneError, "An object could not be cloned.", next);
        greyObject(jsObject);
        if (value->IsDate()) {
            m_writer.writeDate(value.As<v8::Date>()->ValueOf());
        } else if (value->IsStringObject()) {
            writeStringObject(value);
        } else if (value->IsNumberObject()) {
            writeNumberObject(value);
        } else if (value->IsBooleanObject()) {
            writeBooleanObject(value);
        } else if (value->IsArray()) {
            return startArrayState(value.As<v8::Array>(), next);
        } else if (value->IsMap()) {
            return startMapState(value.As<v8::Map>(), next);
        } else if (value->IsSet()) {
            return startSetState(value.As<v8::Set>(), next);
        } else if (V8File::hasInstance(value, isolate())) {
            return writeFile(value, next);
        } else if (V8Blob::hasInstance(value, isolate())) {
            return writeBlob(value, next);
        } else if (V8FileList::hasInstance(value, isolate())) {
            return writeFileList(value, next);
        } else if (V8ImageData::hasInstance(value, isolate())) {
            writeImageData(value);
        } else if (value->IsRegExp()) {
            writeRegExp(value);
        } else if (V8ArrayBuffer::hasInstance(value, isolate())) {
            return writeArrayBuffer(value, next);
        } else if (V8CompositorProxy::hasInstance(value, isolate())) {
            return writeCompositorProxy(value, next);
        } else if (value->IsObject()) {
            if (isHostObject(jsObject) || jsObject->IsCallable() || value->IsNativeError())
                return handleError(DataCloneError, "An object could not be cloned.", next);
            return startObjectState(jsObject, next);
        } else {
            return handleError(DataCloneError, "A value could not be cloned.", next);
        }
    }
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::doSerializeArrayBuffer(v8::Local<v8::Value> arrayBuffer, ScriptValueSerializer::StateBase* next)
{
    return doSerialize(arrayBuffer, next);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::checkException(ScriptValueSerializer::StateBase* state)
{
    return m_tryCatch.HasCaught() ? handleError(JSException, "", state) : 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeObject(uint32_t numProperties, ScriptValueSerializer::StateBase* state)
{
    m_writer.writeObject(numProperties);
    return pop(state);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeSparseArray(uint32_t numProperties, uint32_t length, ScriptValueSerializer::StateBase* state)
{
    m_writer.writeSparseArray(numProperties, length);
    return pop(state);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeDenseArray(uint32_t numProperties, uint32_t length, ScriptValueSerializer::StateBase* state)
{
    m_writer.writeDenseArray(numProperties, length);
    return pop(state);
}

template <>
ScriptValueSerializer::StateBase* ScriptValueSerializer::writeCollection<v8::Map>(uint32_t length, ScriptValueSerializer::StateBase* state)
{
    m_writer.writeMap(length);
    return pop(state);
}

template <>
ScriptValueSerializer::StateBase* ScriptValueSerializer::writeCollection<v8::Set>(uint32_t length, ScriptValueSerializer::StateBase* state)
{
    m_writer.writeSet(length);
    return pop(state);
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::handleError(ScriptValueSerializer::Status errorStatus, const String& message, ScriptValueSerializer::StateBase* state)
{
    ASSERT(errorStatus != Success);
    m_status = errorStatus;
    m_errorMessage = message;
    while (state) {
        StateBase* tmp = state->nextState();
        delete state;
        state = tmp;
    }
    return new ErrorState;
}

bool ScriptValueSerializer::checkComposite(ScriptValueSerializer::StateBase* top)
{
    ASSERT(top);
    if (m_depth > maxDepth)
        return false;
    if (!shouldCheckForCycles(m_depth))
        return true;
    v8::Local<v8::Value> composite = top->composite();
    for (StateBase* state = top->nextState(); state; state = state->nextState()) {
        if (state->composite() == composite)
            return false;
    }
    return true;
}

void ScriptValueSerializer::writeString(v8::Local<v8::Value> value)
{
    v8::Local<v8::String> string = value.As<v8::String>();
    if (!string->Length() || string->IsOneByte())
        m_writer.writeOneByteString(string);
    else
        m_writer.writeUCharString(string);
}

void ScriptValueSerializer::writeStringObject(v8::Local<v8::Value> value)
{
    v8::Local<v8::StringObject> stringObject = value.As<v8::StringObject>();
    v8::String::Utf8Value stringValue(stringObject->ValueOf());
    m_writer.writeStringObject(*stringValue, stringValue.length());
}

void ScriptValueSerializer::writeNumberObject(v8::Local<v8::Value> value)
{
    v8::Local<v8::NumberObject> numberObject = value.As<v8::NumberObject>();
    m_writer.writeNumberObject(numberObject->ValueOf());
}

void ScriptValueSerializer::writeBooleanObject(v8::Local<v8::Value> value)
{
    v8::Local<v8::BooleanObject> booleanObject = value.As<v8::BooleanObject>();
    m_writer.writeBooleanObject(booleanObject->ValueOf());
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeBlob(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    Blob* blob = V8Blob::toImpl(value.As<v8::Object>());
    if (!blob)
        return 0;
    if (blob->hasBeenClosed())
        return handleError(DataCloneError, "A Blob object has been closed, and could therefore not be cloned.", next);
    int blobIndex = -1;
    m_blobDataHandles.set(blob->uuid(), blob->blobDataHandle());
    if (appendBlobInfo(blob->uuid(), blob->type(), blob->size(), &blobIndex))
        m_writer.writeBlobIndex(blobIndex);
    else
        m_writer.writeBlob(blob->uuid(), blob->type(), blob->size());
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeCompositorProxy(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    CompositorProxy* compositorProxy = V8CompositorProxy::toImpl(value.As<v8::Object>());
    if (!compositorProxy)
        return nullptr;
    if (!compositorProxy->connected())
        return handleError(DataCloneError, "A CompositorProxy object has been disconnected, and could therefore not be cloned.", next);
    m_writer.writeCompositorProxy(*compositorProxy);
    return nullptr;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeFile(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    File* file = V8File::toImpl(value.As<v8::Object>());
    if (!file)
        return 0;
    if (file->hasBeenClosed())
        return handleError(DataCloneError, "A File object has been closed, and could therefore not be cloned.", next);
    int blobIndex = -1;
    m_blobDataHandles.set(file->uuid(), file->blobDataHandle());
    if (appendFileInfo(file, &blobIndex)) {
        ASSERT(blobIndex >= 0);
        m_writer.writeFileIndex(blobIndex);
    } else {
        m_writer.writeFile(*file);
    }
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeFileList(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    FileList* fileList = V8FileList::toImpl(value.As<v8::Object>());
    if (!fileList)
        return 0;
    unsigned length = fileList->length();
    Vector<int> blobIndices;
    for (unsigned i = 0; i < length; ++i) {
        int blobIndex = -1;
        const File* file = fileList->item(i);
        if (file->hasBeenClosed())
            return handleError(DataCloneError, "A File object has been closed, and could therefore not be cloned.", next);
        m_blobDataHandles.set(file->uuid(), file->blobDataHandle());
        if (appendFileInfo(file, &blobIndex)) {
            ASSERT(!i || blobIndex > 0);
            ASSERT(blobIndex >= 0);
            blobIndices.append(blobIndex);
        }
    }
    if (!blobIndices.isEmpty())
        m_writer.writeFileListIndex(blobIndices);
    else
        m_writer.writeFileList(*fileList);
    return 0;
}

void ScriptValueSerializer::writeImageData(v8::Local<v8::Value> value)
{
    ImageData* imageData = V8ImageData::toImpl(value.As<v8::Object>());
    if (!imageData)
        return;
    DOMUint8ClampedArray* pixelArray = imageData->data();
    m_writer.writeImageData(imageData->width(), imageData->height(), pixelArray->data(), pixelArray->length());
}

void ScriptValueSerializer::writeRegExp(v8::Local<v8::Value> value)
{
    v8::Local<v8::RegExp> regExp = value.As<v8::RegExp>();
    m_writer.writeRegExp(regExp->GetSource(), regExp->GetFlags());
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeAndGreyArrayBufferView(v8::Local<v8::Object> object, ScriptValueSerializer::StateBase* next)
{
    ASSERT(!object.IsEmpty());
    DOMArrayBufferView* arrayBufferView = V8ArrayBufferView::toImpl(object);
    if (!arrayBufferView)
        return 0;
    if (!arrayBufferView->buffer())
        return handleError(DataCloneError, "An ArrayBuffer could not be cloned.", next);
    v8::Local<v8::Value> underlyingBuffer = toV8(arrayBufferView->buffer(), m_scriptState->context()->Global(), isolate());
    if (underlyingBuffer.IsEmpty())
        return handleError(DataCloneError, "An ArrayBuffer could not be cloned.", next);
    StateBase* stateOut = doSerializeArrayBuffer(underlyingBuffer, next);
    if (stateOut)
        return stateOut;
    m_writer.writeArrayBufferView(*arrayBufferView);
    // This should be safe: we serialize something that we know to be a wrapper (see
    // the toV8 call above), so the call to doSerializeArrayBuffer should neither
    // cause the system stack to overflow nor should it have potential to reach
    // this ArrayBufferView again.
    //
    // We do need to grey the underlying buffer before we grey its view, however;
    // ArrayBuffers may be shared, so they need to be given reference IDs, and an
    // ArrayBufferView cannot be constructed without a corresponding ArrayBuffer
    // (or without an additional tag that would allow us to do two-stage construction
    // like we do for Objects and Arrays).
    greyObject(object);
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeArrayBuffer(v8::Local<v8::Value> value, ScriptValueSerializer::StateBase* next)
{
    DOMArrayBuffer* arrayBuffer = V8ArrayBuffer::toImpl(value.As<v8::Object>());
    if (!arrayBuffer)
        return 0;
    if (arrayBuffer->isNeutered())
        return handleError(DataCloneError, "An ArrayBuffer is neutered and could not be cloned.", next);
    ASSERT(!m_transferredArrayBuffers.contains(value.As<v8::Object>()));
    m_writer.writeArrayBuffer(*arrayBuffer);
    return 0;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::writeTransferredArrayBuffer(v8::Local<v8::Value> value, uint32_t index, ScriptValueSerializer::StateBase* next)
{
    DOMArrayBuffer* arrayBuffer = V8ArrayBuffer::toImpl(value.As<v8::Object>());
    if (!arrayBuffer)
        return 0;
    if (arrayBuffer->isNeutered())
        return handleError(DataCloneError, "An ArrayBuffer is neutered and could not be cloned.", next);
    m_writer.writeTransferredArrayBuffer(index);
    return 0;
}

bool ScriptValueSerializer::shouldSerializeDensely(uint32_t length, uint32_t propertyCount)
{
    // Let K be the cost of serializing all property values that are there
    // Cost of serializing sparsely: 5*propertyCount + K (5 bytes per uint32_t key)
    // Cost of serializing densely: K + 1*(length - propertyCount) (1 byte for all properties that are not there)
    // so densely is better than sparsly whenever 6*propertyCount > length
    return 6 * propertyCount >= length;
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::startArrayState(v8::Local<v8::Array> array, ScriptValueSerializer::StateBase* next)
{
    v8::Local<v8::Array> propertyNames;
    if (!array->GetPropertyNames(context()).ToLocal(&propertyNames))
        return checkException(next);
    uint32_t length = array->Length();

    if (shouldSerializeDensely(length, propertyNames->Length())) {
        m_writer.writeGenerateFreshDenseArray(length);
        return push(new DenseArrayState(array, propertyNames, next, isolate()));
    }

    m_writer.writeGenerateFreshSparseArray(length);
    return push(new SparseArrayState(array, propertyNames, next, isolate()));
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::startMapState(v8::Local<v8::Map> map, ScriptValueSerializer::StateBase* next)
{
    m_writer.writeGenerateFreshMap();
    return push(new MapState(map, next));
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::startSetState(v8::Local<v8::Set> set, ScriptValueSerializer::StateBase* next)
{
    m_writer.writeGenerateFreshSet();
    return push(new SetState(set, next));
}

ScriptValueSerializer::StateBase* ScriptValueSerializer::startObjectState(v8::Local<v8::Object> object, ScriptValueSerializer::StateBase* next)
{
    m_writer.writeGenerateFreshObject();
    // FIXME: check not a wrapper
    return push(new ObjectState(object, next));
}

// Marks object as having been visited by the serializer and assigns it a unique object reference ID.
// An object may only be greyed once.
void ScriptValueSerializer::greyObject(const v8::Local<v8::Object>& object)
{
    ASSERT(!m_objectPool.contains(object));
    uint32_t objectReference = m_nextObjectReference++;
    m_objectPool.set(object, objectReference);
}

bool ScriptValueSerializer::appendBlobInfo(const String& uuid, const String& type, unsigned long long size, int* index)
{
    if (!m_blobInfo)
        return false;
    *index = m_blobInfo->size();
    m_blobInfo->append(WebBlobInfo(uuid, type, size));
    return true;
}

bool ScriptValueSerializer::appendFileInfo(const File* file, int* index)
{
    if (!m_blobInfo)
        return false;

    long long size = -1;
    double lastModifiedMS = invalidFileTime();
    file->captureSnapshot(size, lastModifiedMS);
    *index = m_blobInfo->size();
    // FIXME: transition WebBlobInfo.lastModified to be milliseconds-based also.
    double lastModified = lastModifiedMS / msPerSecond;
    m_blobInfo->append(WebBlobInfo(file->uuid(), file->path(), file->name(), file->type(), lastModified, size));
    return true;
}

bool SerializedScriptValueReader::read(v8::Local<v8::Value>* value, ScriptValueCompositeCreator& creator)
{
    SerializationTag tag;
    if (!readTag(&tag))
        return false;
    return readWithTag(tag, value, creator);
}

bool SerializedScriptValueReader::readWithTag(SerializationTag tag, v8::Local<v8::Value>* value, ScriptValueCompositeCreator& creator)
{
    switch (tag) {
    case ReferenceCountTag: {
        if (!m_version)
            return false;
        uint32_t referenceTableSize;
        if (!doReadUint32(&referenceTableSize))
            return false;
        // If this test fails, then the serializer and deserializer disagree about the assignment
        // of object reference IDs. On the deserialization side, this means there are too many or too few
        // calls to pushObjectReference.
        if (referenceTableSize != creator.objectReferenceCount())
            return false;
        return true;
    }
    case InvalidTag:
        return false;
    case PaddingTag:
        return true;
    case UndefinedTag:
        *value = v8::Undefined(isolate());
        break;
    case NullTag:
        *value = v8::Null(isolate());
        break;
    case TrueTag:
        *value = v8Boolean(true, isolate());
        break;
    case FalseTag:
        *value = v8Boolean(false, isolate());
        break;
    case TrueObjectTag:
        *value = v8::BooleanObject::New(isolate(), true);
        creator.pushObjectReference(*value);
        break;
    case FalseObjectTag:
        *value = v8::BooleanObject::New(isolate(), false);
        creator.pushObjectReference(*value);
        break;
    case StringTag:
        if (!readString(value))
            return false;
        break;
    case StringUCharTag:
        if (!readUCharString(value))
            return false;
        break;
    case StringObjectTag:
        if (!readStringObject(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    case Int32Tag:
        if (!readInt32(value))
            return false;
        break;
    case Uint32Tag:
        if (!readUint32(value))
            return false;
        break;
    case DateTag:
        if (!readDate(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    case NumberTag:
        if (!readNumber(value))
            return false;
        break;
    case NumberObjectTag:
        if (!readNumberObject(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    case BlobTag:
    case BlobIndexTag:
        if (!readBlob(value, tag == BlobIndexTag))
            return false;
        creator.pushObjectReference(*value);
        break;
    case FileTag:
    case FileIndexTag:
        if (!readFile(value, tag == FileIndexTag))
            return false;
        creator.pushObjectReference(*value);
        break;
    case FileListTag:
    case FileListIndexTag:
        if (!readFileList(value, tag == FileListIndexTag))
            return false;
        creator.pushObjectReference(*value);
        break;
    case CompositorProxyTag:
        if (!readCompositorProxy(value))
            return false;
        creator.pushObjectReference(*value);
        break;

    case ImageDataTag:
        if (!readImageData(value))
            return false;
        creator.pushObjectReference(*value);
        break;

    case RegExpTag:
        if (!readRegExp(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    case ObjectTag: {
        uint32_t numProperties;
        if (!doReadUint32(&numProperties))
            return false;
        if (!creator.completeObject(numProperties, value))
            return false;
        break;
    }
    case SparseArrayTag: {
        uint32_t numProperties;
        uint32_t length;
        if (!doReadUint32(&numProperties))
            return false;
        if (!doReadUint32(&length))
            return false;
        if (!creator.completeSparseArray(numProperties, length, value))
            return false;
        break;
    }
    case DenseArrayTag: {
        uint32_t numProperties;
        uint32_t length;
        if (!doReadUint32(&numProperties))
            return false;
        if (!doReadUint32(&length))
            return false;
        if (!creator.completeDenseArray(numProperties, length, value))
            return false;
        break;
    }
    case MapTag: {
        uint32_t length;
        if (!doReadUint32(&length))
            return false;
        if (!creator.completeMap(length, value))
            return false;
        break;
    }
    case SetTag: {
        uint32_t length;
        if (!doReadUint32(&length))
            return false;
        if (!creator.completeSet(length, value))
            return false;
        break;
    }
    case ArrayBufferViewTag: {
        if (!m_version)
            return false;
        if (!readArrayBufferView(value, creator))
            return false;
        creator.pushObjectReference(*value);
        break;
    }
    case ArrayBufferTag: {
        if (!m_version)
            return false;
        if (!readArrayBuffer(value))
            return false;
        creator.pushObjectReference(*value);
        break;
    }
    case GenerateFreshObjectTag: {
        if (!m_version)
            return false;
        if (!creator.newObject())
            return false;
        return true;
    }
    case GenerateFreshSparseArrayTag: {
        if (!m_version)
            return false;
        uint32_t length;
        if (!doReadUint32(&length))
            return false;
        if (!creator.newSparseArray(length))
            return false;
        return true;
    }
    case GenerateFreshDenseArrayTag: {
        if (!m_version)
            return false;
        uint32_t length;
        if (!doReadUint32(&length))
            return false;
        if (!creator.newDenseArray(length))
            return false;
        return true;
    }
    case GenerateFreshMapTag: {
        if (!m_version)
            return false;
        if (!creator.newMap())
            return false;
        return true;
    }
    case GenerateFreshSetTag: {
        if (!m_version)
            return false;
        if (!creator.newSet())
            return false;
        return true;
    }
    case MessagePortTag: {
        if (!m_version)
            return false;
        uint32_t index;
        if (!doReadUint32(&index))
            return false;
        if (!creator.tryGetTransferredMessagePort(index, value))
            return false;
        break;
    }
    case ArrayBufferTransferTag: {
        if (!m_version)
            return false;
        uint32_t index;
        if (!doReadUint32(&index))
            return false;
        if (!creator.tryGetTransferredArrayBuffer(index, value))
            return false;
        break;
    }
    case ObjectReferenceTag: {
        if (!m_version)
            return false;
        uint32_t reference;
        if (!doReadUint32(&reference))
            return false;
        if (!creator.tryGetObjectFromObjectReference(reference, value))
            return false;
        break;
    }
    case DOMFileSystemTag:
    case CryptoKeyTag:
        ASSERT_NOT_REACHED();
    default:
        return false;
    }
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readVersion(uint32_t& version)
{
    SerializationTag tag;
    if (!readTag(&tag)) {
        // This is a nullary buffer. We're still version 0.
        version = 0;
        return true;
    }
    if (tag != VersionTag) {
        // Versions of the format past 0 start with the version tag.
        version = 0;
        // Put back the tag.
        undoReadTag();
        return true;
    }
    // Version-bearing messages are obligated to finish the version tag.
    return doReadUint32(&version);
}

void SerializedScriptValueReader::setVersion(uint32_t version)
{
    m_version = version;
}

bool SerializedScriptValueReader::readTag(SerializationTag* tag)
{
    if (m_position >= m_length)
        return false;
    *tag = static_cast<SerializationTag>(m_buffer[m_position++]);
    return true;
}

void SerializedScriptValueReader::undoReadTag()
{
    if (m_position > 0)
        --m_position;
}

bool SerializedScriptValueReader::readArrayBufferViewSubTag(ArrayBufferViewSubTag* tag)
{
    if (m_position >= m_length)
        return false;
    *tag = static_cast<ArrayBufferViewSubTag>(m_buffer[m_position++]);
    return true;
}

bool SerializedScriptValueReader::readString(v8::Local<v8::Value>* value)
{
    uint32_t length;
    if (!doReadUint32(&length))
        return false;
    if (m_position + length > m_length)
        return false;
    *value = v8AtomicString(isolate(), reinterpret_cast<const char*>(m_buffer + m_position), length);
    m_position += length;
    return true;
}

bool SerializedScriptValueReader::readUCharString(v8::Local<v8::Value>* value)
{
    uint32_t length;
    if (!doReadUint32(&length) || (length & 1))
        return false;
    if (m_position + length > m_length)
        return false;
    ASSERT(!(m_position & 1));
    if (!v8::String::NewFromTwoByte(isolate(), reinterpret_cast<const uint16_t*>(m_buffer + m_position), v8::NewStringType::kNormal, length / sizeof(UChar)).ToLocal(value))
        return false;
    m_position += length;
    return true;
}

bool SerializedScriptValueReader::readStringObject(v8::Local<v8::Value>* value)
{
    v8::Local<v8::Value> stringValue;
    if (!readString(&stringValue) || !stringValue->IsString())
        return false;

    *value = v8::StringObject::New(isolate(), stringValue.As<v8::String>());
    return true;
}

bool SerializedScriptValueReader::readWebCoreString(String* string)
{
    uint32_t length;
    if (!doReadUint32(&length))
        return false;
    if (m_position + length > m_length)
        return false;
    *string = String::fromUTF8(reinterpret_cast<const char*>(m_buffer + m_position), length);
    m_position += length;
    return true;
}

bool SerializedScriptValueReader::readInt32(v8::Local<v8::Value>* value)
{
    uint32_t rawValue;
    if (!doReadUint32(&rawValue))
        return false;
    *value = v8::Integer::New(isolate(), static_cast<int32_t>(ZigZag::decode(rawValue)));
    return true;
}

bool SerializedScriptValueReader::readUint32(v8::Local<v8::Value>* value)
{
    uint32_t rawValue;
    if (!doReadUint32(&rawValue))
        return false;
    *value = v8::Integer::NewFromUnsigned(isolate(), rawValue);
    return true;
}

bool SerializedScriptValueReader::readDate(v8::Local<v8::Value>* value)
{
    double numberValue;
    if (!doReadNumber(&numberValue))
        return false;
    if (!v8DateOrNaN(isolate(), numberValue).ToLocal(value))
        return false;
    return true;
}

bool SerializedScriptValueReader::readNumber(v8::Local<v8::Value>* value)
{
    double number;
    if (!doReadNumber(&number))
        return false;
    *value = v8::Number::New(isolate(), number);
    return true;
}

bool SerializedScriptValueReader::readNumberObject(v8::Local<v8::Value>* value)
{
    double number;
    if (!doReadNumber(&number))
        return false;
    *value = v8::NumberObject::New(isolate(), number);
    return true;
}

bool SerializedScriptValueReader::readImageData(v8::Local<v8::Value>* value)
{
    uint32_t width;
    uint32_t height;
    uint32_t pixelDataLength;
    if (!doReadUint32(&width))
        return false;
    if (!doReadUint32(&height))
        return false;
    if (!doReadUint32(&pixelDataLength))
        return false;
    if (m_position + pixelDataLength > m_length)
        return false;
    ImageData* imageData = ImageData::create(IntSize(width, height));
    DOMUint8ClampedArray* pixelArray = imageData->data();
    ASSERT(pixelArray);
    ASSERT(pixelArray->length() >= pixelDataLength);
    memcpy(pixelArray->data(), m_buffer + m_position, pixelDataLength);
    m_position += pixelDataLength;
    *value = toV8(imageData, m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readCompositorProxy(v8::Local<v8::Value>* value)
{
    uint32_t attributes;
    uint64_t element;
    if (!doReadUint64(&element))
        return false;
    if (!doReadUint32(&attributes))
        return false;

    CompositorProxy* compositorProxy = CompositorProxy::create(element, attributes);
    *value = toV8(compositorProxy, m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

PassRefPtr<DOMArrayBuffer> SerializedScriptValueReader::doReadArrayBuffer()
{
    uint32_t byteLength;
    if (!doReadUint32(&byteLength))
        return nullptr;
    if (m_position + byteLength > m_length)
        return nullptr;
    const void* bufferStart = m_buffer + m_position;
    m_position += byteLength;
    return DOMArrayBuffer::create(bufferStart, byteLength);
}

bool SerializedScriptValueReader::readArrayBuffer(v8::Local<v8::Value>* value)
{
    RefPtr<DOMArrayBuffer> arrayBuffer = doReadArrayBuffer();
    if (!arrayBuffer)
        return false;
    *value = toV8(arrayBuffer.release(), m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readArrayBufferView(v8::Local<v8::Value>* value, ScriptValueCompositeCreator& creator)
{
    ArrayBufferViewSubTag subTag;
    uint32_t byteOffset;
    uint32_t byteLength;
    RefPtr<DOMArrayBuffer> arrayBuffer;
    v8::Local<v8::Value> arrayBufferV8Value;
    if (!readArrayBufferViewSubTag(&subTag))
        return false;
    if (!doReadUint32(&byteOffset))
        return false;
    if (!doReadUint32(&byteLength))
        return false;
    if (!creator.consumeTopOfStack(&arrayBufferV8Value))
        return false;
    if (arrayBufferV8Value.IsEmpty())
        return false;
    arrayBuffer = V8ArrayBuffer::toImpl(arrayBufferV8Value.As<v8::Object>());
    if (!arrayBuffer)
        return false;

    // Check the offset, length and alignment.
    int elementByteSize;
    switch (subTag) {
    case ByteArrayTag:
        elementByteSize = sizeof(DOMInt8Array::ValueType);
        break;
    case UnsignedByteArrayTag:
        elementByteSize = sizeof(DOMUint8Array::ValueType);
        break;
    case UnsignedByteClampedArrayTag:
        elementByteSize = sizeof(DOMUint8ClampedArray::ValueType);
        break;
    case ShortArrayTag:
        elementByteSize = sizeof(DOMInt16Array::ValueType);
        break;
    case UnsignedShortArrayTag:
        elementByteSize = sizeof(DOMUint16Array::ValueType);
        break;
    case IntArrayTag:
        elementByteSize = sizeof(DOMInt32Array::ValueType);
        break;
    case UnsignedIntArrayTag:
        elementByteSize = sizeof(DOMUint32Array::ValueType);
        break;
    case FloatArrayTag:
        elementByteSize = sizeof(DOMFloat32Array::ValueType);
        break;
    case DoubleArrayTag:
        elementByteSize = sizeof(DOMFloat64Array::ValueType);
        break;
    case DataViewTag:
        elementByteSize = sizeof(DOMDataView::ValueType);
        break;
    default:
        return false;
    }
    const unsigned numElements = byteLength / elementByteSize;
    const unsigned remainingElements = (arrayBuffer->byteLength() - byteOffset) / elementByteSize;
    if (byteOffset % elementByteSize
        || byteOffset > arrayBuffer->byteLength()
        || numElements > remainingElements)
        return false;

    v8::Local<v8::Object> creationContext = m_scriptState->context()->Global();
    switch (subTag) {
    case ByteArrayTag:
        *value = toV8(DOMInt8Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case UnsignedByteArrayTag:
        *value = toV8(DOMUint8Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case UnsignedByteClampedArrayTag:
        *value = toV8(DOMUint8ClampedArray::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case ShortArrayTag:
        *value = toV8(DOMInt16Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case UnsignedShortArrayTag:
        *value = toV8(DOMUint16Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case IntArrayTag:
        *value = toV8(DOMInt32Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case UnsignedIntArrayTag:
        *value = toV8(DOMUint32Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case FloatArrayTag:
        *value = toV8(DOMFloat32Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case DoubleArrayTag:
        *value = toV8(DOMFloat64Array::create(arrayBuffer.release(), byteOffset, numElements), creationContext, isolate());
        break;
    case DataViewTag:
        *value = toV8(DOMDataView::create(arrayBuffer.release(), byteOffset, byteLength), creationContext, isolate());
        break;
    }
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readRegExp(v8::Local<v8::Value>* value)
{
    v8::Local<v8::Value> pattern;
    if (!readString(&pattern))
        return false;
    uint32_t flags;
    if (!doReadUint32(&flags))
        return false;
    if (!v8::RegExp::New(scriptState()->context(), pattern.As<v8::String>(), static_cast<v8::RegExp::Flags>(flags)).ToLocal(value))
        return false;
    return true;
}

bool SerializedScriptValueReader::readBlob(v8::Local<v8::Value>* value, bool isIndexed)
{
    if (m_version < 3)
        return false;
    Blob* blob = nullptr;
    if (isIndexed) {
        if (m_version < 6)
            return false;
        ASSERT(m_blobInfo);
        uint32_t index;
        if (!doReadUint32(&index) || index >= m_blobInfo->size())
            return false;
        const WebBlobInfo& info = (*m_blobInfo)[index];
        blob = Blob::create(getOrCreateBlobDataHandle(info.uuid(), info.type(), info.size()));
    } else {
        ASSERT(!m_blobInfo);
        String uuid;
        String type;
        uint64_t size;
        ASSERT(!m_blobInfo);
        if (!readWebCoreString(&uuid))
            return false;
        if (!readWebCoreString(&type))
            return false;
        if (!doReadUint64(&size))
            return false;
        blob = Blob::create(getOrCreateBlobDataHandle(uuid, type, size));
    }
    *value = toV8(blob, m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readFile(v8::Local<v8::Value>* value, bool isIndexed)
{
    File* file = nullptr;
    if (isIndexed) {
        if (m_version < 6)
            return false;
        file = readFileIndexHelper();
    } else {
        file = readFileHelper();
    }
    if (!file)
        return false;
    *value = toV8(file, m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

bool SerializedScriptValueReader::readFileList(v8::Local<v8::Value>* value, bool isIndexed)
{
    if (m_version < 3)
        return false;
    uint32_t length;
    if (!doReadUint32(&length))
        return false;
    FileList* fileList = FileList::create();
    for (unsigned i = 0; i < length; ++i) {
        File* file = nullptr;
        if (isIndexed) {
            if (m_version < 6)
                return false;
            file = readFileIndexHelper();
        } else {
            file = readFileHelper();
        }
        if (!file)
            return false;
        fileList->append(file);
    }
    *value = toV8(fileList, m_scriptState->context()->Global(), isolate());
    return !value->IsEmpty();
}

File* SerializedScriptValueReader::readFileHelper()
{
    if (m_version < 3)
        return nullptr;
    ASSERT(!m_blobInfo);
    String path;
    String name;
    String relativePath;
    String uuid;
    String type;
    uint32_t hasSnapshot = 0;
    uint64_t size = 0;
    double lastModifiedMS = 0;
    if (!readWebCoreString(&path))
        return nullptr;
    if (m_version >= 4 && !readWebCoreString(&name))
        return nullptr;
    if (m_version >= 4 && !readWebCoreString(&relativePath))
        return nullptr;
    if (!readWebCoreString(&uuid))
        return nullptr;
    if (!readWebCoreString(&type))
        return nullptr;
    if (m_version >= 4 && !doReadUint32(&hasSnapshot))
        return nullptr;
    if (hasSnapshot) {
        if (!doReadUint64(&size))
            return nullptr;
        if (!doReadNumber(&lastModifiedMS))
            return nullptr;
        if (m_version < 8)
            lastModifiedMS *= msPerSecond;
    }
    uint32_t isUserVisible = 1;
    if (m_version >= 7 && !doReadUint32(&isUserVisible))
        return nullptr;
    const File::UserVisibility userVisibility = (isUserVisible > 0) ? File::IsUserVisible : File::IsNotUserVisible;
    return File::createFromSerialization(path, name, relativePath, userVisibility, hasSnapshot > 0, size, lastModifiedMS, getOrCreateBlobDataHandle(uuid, type));
}

File* SerializedScriptValueReader::readFileIndexHelper()
{
    if (m_version < 3)
        return nullptr;
    ASSERT(m_blobInfo);
    uint32_t index;
    if (!doReadUint32(&index) || index >= m_blobInfo->size())
        return nullptr;
    const WebBlobInfo& info = (*m_blobInfo)[index];
    // FIXME: transition WebBlobInfo.lastModified to be milliseconds-based also.
    double lastModifiedMS = info.lastModified() * msPerSecond;
    return File::createFromIndexedSerialization(info.filePath(), info.fileName(), info.size(), lastModifiedMS, getOrCreateBlobDataHandle(info.uuid(), info.type(), info.size()));
}

bool SerializedScriptValueReader::doReadUint32(uint32_t* value)
{
    return doReadUintHelper(value);
}

bool SerializedScriptValueReader::doReadUint64(uint64_t* value)
{
    return doReadUintHelper(value);
}

bool SerializedScriptValueReader::doReadNumber(double* number)
{
    if (m_position + sizeof(double) > m_length)
        return false;
    uint8_t* numberAsByteArray = reinterpret_cast<uint8_t*>(number);
    for (unsigned i = 0; i < sizeof(double); ++i)
        numberAsByteArray[i] = m_buffer[m_position++];
    return true;
}

PassRefPtr<BlobDataHandle> SerializedScriptValueReader::getOrCreateBlobDataHandle(const String& uuid, const String& type, long long size)
{
    // The containing ssv may have a BDH for this uuid if this ssv is just being
    // passed from main to worker thread (for example). We use those values when creating
    // the new blob instead of cons'ing up a new BDH.
    //
    // FIXME: Maybe we should require that it work that way where the ssv must have a BDH for any
    // blobs it comes across during deserialization. Would require callers to explicitly populate
    // the collection of BDH's for blobs to work, which would encourage lifetimes to be considered
    // when passing ssv's around cross process. At present, we get 'lucky' in some cases because
    // the blob in the src process happens to still exist at the time the dest process is deserializing.
    // For example in sharedWorker.postMessage(...).
    BlobDataHandleMap::const_iterator it = m_blobDataHandles.find(uuid);
    if (it != m_blobDataHandles.end()) {
        // make assertions about type and size?
        return it->value;
    }
    return BlobDataHandle::create(uuid, type, size);
}

v8::Local<v8::Value> ScriptValueDeserializer::deserialize()
{
    v8::Isolate* isolate = m_reader.scriptState()->isolate();
    if (!m_reader.readVersion(m_version) || m_version > SerializedScriptValue::wireFormatVersion)
        return v8::Null(isolate);
    m_reader.setVersion(m_version);
    v8::EscapableHandleScope scope(isolate);
    while (!m_reader.isEof()) {
        if (!doDeserialize())
            return v8::Null(isolate);
    }
    if (stackDepth() != 1 || m_openCompositeReferenceStack.size())
        return v8::Null(isolate);
    v8::Local<v8::Value> result = scope.Escape(element(0));
    return result;
}

bool ScriptValueDeserializer::newSparseArray(uint32_t)
{
    v8::Local<v8::Array> array = v8::Array::New(m_reader.scriptState()->isolate(), 0);
    openComposite(array);
    return true;
}

bool ScriptValueDeserializer::newDenseArray(uint32_t length)
{
    v8::Local<v8::Array> array = v8::Array::New(m_reader.scriptState()->isolate(), length);
    openComposite(array);
    return true;
}

bool ScriptValueDeserializer::newMap()
{
    v8::Local<v8::Map> map = v8::Map::New(m_reader.scriptState()->isolate());
    openComposite(map);
    return true;
}

bool ScriptValueDeserializer::newSet()
{
    v8::Local<v8::Set> set = v8::Set::New(m_reader.scriptState()->isolate());
    openComposite(set);
    return true;
}

bool ScriptValueDeserializer::consumeTopOfStack(v8::Local<v8::Value>* object)
{
    if (stackDepth() < 1)
        return false;
    *object = element(stackDepth() - 1);
    pop(1);
    return true;
}

bool ScriptValueDeserializer::newObject()
{
    v8::Local<v8::Object> object = v8::Object::New(m_reader.scriptState()->isolate());
    if (object.IsEmpty())
        return false;
    openComposite(object);
    return true;
}

bool ScriptValueDeserializer::completeObject(uint32_t numProperties, v8::Local<v8::Value>* value)
{
    v8::Local<v8::Object> object;
    if (m_version > 0) {
        v8::Local<v8::Value> composite;
        if (!closeComposite(&composite))
            return false;
        object = composite.As<v8::Object>();
    } else {
        object = v8::Object::New(m_reader.scriptState()->isolate());
    }
    if (object.IsEmpty())
        return false;
    return initializeObject(object, numProperties, value);
}

bool ScriptValueDeserializer::completeSparseArray(uint32_t numProperties, uint32_t length, v8::Local<v8::Value>* value)
{
    v8::Local<v8::Array> array;
    if (m_version > 0) {
        v8::Local<v8::Value> composite;
        if (!closeComposite(&composite))
            return false;
        array = composite.As<v8::Array>();
    } else {
        array = v8::Array::New(m_reader.scriptState()->isolate());
    }
    if (array.IsEmpty())
        return false;
    return initializeObject(array, numProperties, value);
}

bool ScriptValueDeserializer::completeDenseArray(uint32_t numProperties, uint32_t length, v8::Local<v8::Value>* value)
{
    v8::Local<v8::Array> array;
    if (m_version > 0) {
        v8::Local<v8::Value> composite;
        if (!closeComposite(&composite))
            return false;
        array = composite.As<v8::Array>();
    }
    if (array.IsEmpty())
        return false;
    if (!initializeObject(array, numProperties, value))
        return false;
    if (length > stackDepth())
        return false;
    v8::Local<v8::Context> context = m_reader.scriptState()->context();
    for (unsigned i = 0, stackPos = stackDepth() - length; i < length; i++, stackPos++) {
        v8::Local<v8::Value> elem = element(stackPos);
        if (!elem->IsUndefined()) {
            if (!v8CallBoolean(array->CreateDataProperty(context, i, elem)))
                return false;
        }
    }
    pop(length);
    return true;
}

bool ScriptValueDeserializer::completeMap(uint32_t length, v8::Local<v8::Value>* value)
{
    ASSERT(m_version > 0);
    v8::Local<v8::Value> composite;
    if (!closeComposite(&composite))
        return false;
    v8::Local<v8::Map> map = composite.As<v8::Map>();
    if (map.IsEmpty())
        return false;
    v8::Local<v8::Context> context = m_reader.scriptState()->context();
    ASSERT(length % 2 == 0);
    for (unsigned i = stackDepth() - length; i + 1 < stackDepth(); i += 2) {
        v8::Local<v8::Value> key = element(i);
        v8::Local<v8::Value> val = element(i + 1);
        if (map->Set(context, key, val).IsEmpty())
            return false;
    }
    pop(length);
    *value = map;
    return true;
}

bool ScriptValueDeserializer::completeSet(uint32_t length, v8::Local<v8::Value>* value)
{
    ASSERT(m_version > 0);
    v8::Local<v8::Value> composite;
    if (!closeComposite(&composite))
        return false;
    v8::Local<v8::Set> set = composite.As<v8::Set>();
    if (set.IsEmpty())
        return false;
    v8::Local<v8::Context> context = m_reader.scriptState()->context();
    for (unsigned i = stackDepth() - length; i < stackDepth(); i++) {
        v8::Local<v8::Value> key = element(i);
        if (set->Add(context, key).IsEmpty())
            return false;
    }
    pop(length);
    *value = set;
    return true;
}

void ScriptValueDeserializer::pushObjectReference(const v8::Local<v8::Value>& object)
{
    m_objectPool.append(object);
}

bool ScriptValueDeserializer::tryGetTransferredMessagePort(uint32_t index, v8::Local<v8::Value>* object)
{
    if (!m_transferredMessagePorts)
        return false;
    if (index >= m_transferredMessagePorts->size())
        return false;
    v8::Local<v8::Object> creationContext = m_reader.scriptState()->context()->Global();
    *object = toV8(m_transferredMessagePorts->at(index).get(), creationContext, m_reader.scriptState()->isolate());
    return !object->IsEmpty();
}

bool ScriptValueDeserializer::tryGetTransferredArrayBuffer(uint32_t index, v8::Local<v8::Value>* object)
{
    if (!m_arrayBufferContents)
        return false;
    if (index >= m_arrayBuffers.size())
        return false;
    v8::Local<v8::Value> result = m_arrayBuffers.at(index);
    if (result.IsEmpty()) {
        RefPtr<DOMArrayBuffer> buffer = DOMArrayBuffer::create(m_arrayBufferContents->at(index));
        v8::Isolate* isolate = m_reader.scriptState()->isolate();
        v8::Local<v8::Object> creationContext = m_reader.scriptState()->context()->Global();
        result = toV8(buffer.get(), creationContext, isolate);
        if (result.IsEmpty())
            return false;
        m_arrayBuffers[index] = result;
    }
    *object = result;
    return true;
}

bool ScriptValueDeserializer::tryGetObjectFromObjectReference(uint32_t reference, v8::Local<v8::Value>* object)
{
    if (reference >= m_objectPool.size())
        return false;
    *object = m_objectPool[reference];
    return object;
}

uint32_t ScriptValueDeserializer::objectReferenceCount()
{
    return m_objectPool.size();
}

bool ScriptValueDeserializer::initializeObject(v8::Local<v8::Object> object, uint32_t numProperties, v8::Local<v8::Value>* value)
{
    unsigned length = 2 * numProperties;
    if (length > stackDepth())
        return false;
    v8::Local<v8::Context> context = m_reader.scriptState()->context();
    for (unsigned i = stackDepth() - length; i < stackDepth(); i += 2) {
        v8::Local<v8::Value> propertyName = element(i);
        v8::Local<v8::Value> propertyValue = element(i + 1);
        bool result = false;
        if (propertyName->IsString())
            result = v8CallBoolean(object->CreateDataProperty(context, propertyName.As<v8::String>(), propertyValue));
        else if (propertyName->IsUint32())
            result = v8CallBoolean(object->CreateDataProperty(context, propertyName.As<v8::Uint32>()->Value(), propertyValue));
        else
            ASSERT_NOT_REACHED();
        if (!result)
            return false;
    }
    pop(length);
    *value = object;
    return true;
}

bool ScriptValueDeserializer::read(v8::Local<v8::Value>* value)
{
    return m_reader.read(value, *this);
}

bool ScriptValueDeserializer::doDeserialize()
{
    v8::Local<v8::Value> value;
    if (!read(&value))
        return false;
    if (!value.IsEmpty())
        push(value);
    return true;
}

v8::Local<v8::Value> ScriptValueDeserializer::element(unsigned index)
{
    ASSERT_WITH_SECURITY_IMPLICATION(index < m_stack.size());
    return m_stack[index];
}

void ScriptValueDeserializer::openComposite(const v8::Local<v8::Value>& object)
{
    uint32_t newObjectReference = m_objectPool.size();
    m_openCompositeReferenceStack.append(newObjectReference);
    m_objectPool.append(object);
}

bool ScriptValueDeserializer::closeComposite(v8::Local<v8::Value>* object)
{
    if (!m_openCompositeReferenceStack.size())
        return false;
    uint32_t objectReference = m_openCompositeReferenceStack[m_openCompositeReferenceStack.size() - 1];
    m_openCompositeReferenceStack.shrink(m_openCompositeReferenceStack.size() - 1);
    if (objectReference >= m_objectPool.size())
        return false;
    *object = m_objectPool[objectReference];
    return true;
}

} // namespace blink
