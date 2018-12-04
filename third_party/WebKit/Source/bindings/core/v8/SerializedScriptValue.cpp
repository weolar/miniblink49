/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "bindings/core/v8/SerializedScriptValue.h"

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValueSerializer.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8MessagePort.h"
#include "core/dom/ExceptionCode.h"
#include "platform/SharedBuffer.h"
#include "platform/blob/BlobData.h"
#include "platform/heap/Handle.h"
#include "wtf/Assertions.h"
#include "wtf/ByteOrder.h"
#include "wtf/Vector.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/StringHash.h"

namespace blink {

PassRefPtr<SerializedScriptValue> SerializedScriptValue::nullValue()
{
    SerializedScriptValueWriter writer;
    writer.writeNull();
    String wireData = writer.takeWireString();
    return adoptRef(new SerializedScriptValue(wireData));
}

// Convert serialized string to big endian wire data.
void SerializedScriptValue::toWireBytes(Vector<char>& result) const
{
    ASSERT(result.isEmpty());
    size_t length = m_data.length();
    result.resize(length * sizeof(UChar));
    UChar* dst = reinterpret_cast<UChar*>(result.data());

    if (m_data.is8Bit()) {
        const LChar* src = m_data.characters8();
        for (size_t i = 0; i < length; i++)
            dst[i] = htons(static_cast<UChar>(src[i]));
    } else {
        const UChar* src = m_data.characters16();
        for (size_t i = 0; i < length; i++)
            dst[i] = htons(src[i]);
    }
}

SerializedScriptValue::SerializedScriptValue()
    : m_externallyAllocatedMemory(0)
{
}

static void acculumateArrayBuffersForAllWorlds(v8::Isolate* isolate, DOMArrayBuffer* object, Vector<v8::Local<v8::ArrayBuffer>, 4>& buffers)
{
    if (isMainThread()) {
        Vector<RefPtr<DOMWrapperWorld>> worlds;
        DOMWrapperWorld::allWorldsInMainThread(worlds);
        for (size_t i = 0; i < worlds.size(); i++) {
            v8::Local<v8::Object> wrapper = worlds[i]->domDataStore().get(object, isolate);
            if (!wrapper.IsEmpty())
                buffers.append(v8::Local<v8::ArrayBuffer>::Cast(wrapper));
        }
    } else {
        v8::Local<v8::Object> wrapper = DOMWrapperWorld::current(isolate).domDataStore().get(object, isolate);
        if (!wrapper.IsEmpty())
            buffers.append(v8::Local<v8::ArrayBuffer>::Cast(wrapper));
    }
}

PassOwnPtr<SerializedScriptValue::ArrayBufferContentsArray> SerializedScriptValue::createArrayBuffers(v8::Isolate* isolate, ArrayBufferArray& arrayBuffers, ExceptionState& exceptionState)
{
    ASSERT(arrayBuffers.size());

    for (size_t i = 0; i < arrayBuffers.size(); i++) {
        if (arrayBuffers[i]->isNeutered()) {
            exceptionState.throwDOMException(DataCloneError, "ArrayBuffer at index " + String::number(i) + " is already neutered.");
            return nullptr;
        }
    }

    OwnPtr<ArrayBufferContentsArray> contents = adoptPtr(new ArrayBufferContentsArray(arrayBuffers.size()));

    HashSet<DOMArrayBuffer*> visited;
    for (size_t i = 0; i < arrayBuffers.size(); i++) {
        if (visited.contains(arrayBuffers[i].get()))
            continue;
        visited.add(arrayBuffers[i].get());

        Vector<v8::Local<v8::ArrayBuffer>, 4> bufferHandles;
        v8::HandleScope handleScope(isolate);
        acculumateArrayBuffersForAllWorlds(isolate, arrayBuffers[i].get(), bufferHandles);
        bool isNeuterable = true;
        for (size_t j = 0; j < bufferHandles.size(); j++)
            isNeuterable &= bufferHandles[j]->IsNeuterable();

        RefPtr<DOMArrayBuffer> toTransfer = arrayBuffers[i];
        if (!isNeuterable)
            toTransfer = DOMArrayBuffer::create(arrayBuffers[i]->buffer());
        bool result = toTransfer->transfer(contents->at(i));
        if (!result) {
            exceptionState.throwDOMException(DataCloneError, "ArrayBuffer at index " + String::number(i) + " could not be transferred.");
            return nullptr;
        }

        if (isNeuterable)
            for (size_t j = 0; j < bufferHandles.size(); j++)
                bufferHandles[j]->Neuter();
    }
    return contents.release();
}

SerializedScriptValue::SerializedScriptValue(const String& wireData)
    : m_externallyAllocatedMemory(0)
{
    m_data = wireData.isolatedCopy();
}

v8::Local<v8::Value> SerializedScriptValue::deserialize(MessagePortArray* messagePorts)
{
    return deserialize(v8::Isolate::GetCurrent(), messagePorts, 0);
}

v8::Local<v8::Value> SerializedScriptValue::deserialize(v8::Isolate* isolate, MessagePortArray* messagePorts, const WebBlobInfoArray* blobInfo)
{
    return SerializedScriptValueFactory::instance().deserialize(this, isolate, messagePorts, blobInfo);
}

bool SerializedScriptValue::extractTransferables(v8::Isolate* isolate, v8::Local<v8::Value> value, int argumentIndex, MessagePortArray& ports, ArrayBufferArray& arrayBuffers, ExceptionState& exceptionState)
{
    if (isUndefinedOrNull(value)) {
        ports.resize(0);
        arrayBuffers.resize(0);
        return true;
    }

    uint32_t length = 0;
    if (value->IsArray()) {
        v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(value);
        length = array->Length();
    } else if (!toV8Sequence(value, length, isolate, exceptionState)) {
        if (!exceptionState.hadException())
            exceptionState.throwTypeError(ExceptionMessages::notAnArrayTypeArgumentOrValue(argumentIndex + 1));
        return false;
    }

    v8::Local<v8::Object> transferrables = v8::Local<v8::Object>::Cast(value);

    // Validate the passed array of transferrables.
    for (unsigned i = 0; i < length; ++i) {
        v8::Local<v8::Value> transferrable;
        if (!transferrables->Get(isolate->GetCurrentContext(), i).ToLocal(&transferrable))
            return false;
        // Validation of non-null objects, per HTML5 spec 10.3.3.
        if (isUndefinedOrNull(transferrable)) {
            exceptionState.throwTypeError("Value at index " + String::number(i) + " is an untransferable " + (transferrable->IsUndefined() ? "'undefined'" : "'null'") + " value.");
            return false;
        }
        // Validation of Objects implementing an interface, per WebIDL spec 4.1.15.
        if (V8MessagePort::hasInstance(transferrable, isolate)) {
            MessagePort* port = V8MessagePort::toImpl(v8::Local<v8::Object>::Cast(transferrable));
            // Check for duplicate MessagePorts.
            if (ports.contains(port)) {
                exceptionState.throwDOMException(DataCloneError, "Message port at index " + String::number(i) + " is a duplicate of an earlier port.");
                return false;
            }
            ports.append(port);
        } else if (V8ArrayBuffer::hasInstance(transferrable, isolate)) {
            RefPtr<DOMArrayBuffer> arrayBuffer = V8ArrayBuffer::toImpl(v8::Local<v8::Object>::Cast(transferrable));
            if (arrayBuffers.contains(arrayBuffer)) {
                exceptionState.throwDOMException(DataCloneError, "ArrayBuffer at index " + String::number(i) + " is a duplicate of an earlier ArrayBuffer.");
                return false;
            }
            arrayBuffers.append(arrayBuffer.release());
        } else {
            exceptionState.throwTypeError("Value at index " + String::number(i) + " does not have a transferable type.");
            return false;
        }
    }
    return true;
}

void blinkAdjustAmountOfExternalAllocatedMemory(int size);

void SerializedScriptValue::registerMemoryAllocatedWithCurrentScriptContext()
{
    if (m_externallyAllocatedMemory)
        return;
    m_externallyAllocatedMemory = static_cast<intptr_t>(m_data.length());
    blinkAdjustAmountOfExternalAllocatedMemory(m_externallyAllocatedMemory);
}

SerializedScriptValue::~SerializedScriptValue()
{
    // If the allocated memory was not registered before, then this class is likely
    // used in a context other then Worker's onmessage environment and the presence of
    // current v8 context is not guaranteed. Avoid calling v8 then.
    if (m_externallyAllocatedMemory) {
        blinkAdjustAmountOfExternalAllocatedMemory(-m_externallyAllocatedMemory);
    }
}

void SerializedScriptValue::transferArrayBuffers(v8::Isolate* isolate, ArrayBufferArray& arrayBuffers, ExceptionState& exceptionState)
{
    m_arrayBufferContentsArray = createArrayBuffers(isolate, arrayBuffers, exceptionState);
}

} // namespace blink
