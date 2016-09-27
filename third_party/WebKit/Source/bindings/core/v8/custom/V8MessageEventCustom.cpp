/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8MessageEvent.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8MessagePort.h"
#include "bindings/core/v8/V8Window.h"
#include "core/events/MessageEvent.h"

namespace blink {

void V8MessageEvent::dataAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Value> cachedData = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::data(info.GetIsolate()));
    if (!cachedData.IsEmpty()) {
        v8SetReturnValue(info, cachedData);
        return;
    }

    MessageEvent* event = V8MessageEvent::toImpl(info.Holder());

    v8::Local<v8::Value> result;
    switch (event->dataType()) {
    case MessageEvent::DataTypeScriptValue:
        result = event->dataAsScriptValue().v8ValueFor(ScriptState::current(info.GetIsolate()));
        if (result.IsEmpty())
            result = v8::Null(info.GetIsolate());
        break;

    case MessageEvent::DataTypeSerializedScriptValue:
        if (SerializedScriptValue* serializedValue = event->dataAsSerializedScriptValue()) {
            MessagePortArray ports = event->ports();
            result = serializedValue->deserialize(info.GetIsolate(), &ports);
        } else {
            result = v8::Null(info.GetIsolate());
        }
        break;

    case MessageEvent::DataTypeString: {
        result = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::stringData(info.GetIsolate()));
        if (result.IsEmpty()) {
            String stringValue = event->dataAsString();
            result = v8String(info.GetIsolate(), stringValue);
        }
        break;
    }

    case MessageEvent::DataTypeBlob:
        result = toV8(event->dataAsBlob(), info.Holder(), info.GetIsolate());
        break;

    case MessageEvent::DataTypeArrayBuffer:
        result = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::arrayBufferData(info.GetIsolate()));
        if (result.IsEmpty())
            result = toV8(event->dataAsArrayBuffer(), info.Holder(), info.GetIsolate());
        break;
    }

    // Store the result as a hidden value so this callback returns the cached
    // result in future invocations.
    V8HiddenValue::setHiddenValue(info.GetIsolate(),  info.Holder(), V8HiddenValue::data(info.GetIsolate()), result);
    v8SetReturnValue(info, result);
}

void V8MessageEvent::initMessageEventMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "initMessageEvent", "MessageEvent", info.Holder(), info.GetIsolate());
    MessageEvent* event = V8MessageEvent::toImpl(info.Holder());
    TOSTRING_VOID(V8StringResource<>, typeArg, info[0]);
    bool canBubbleArg;
    bool cancelableArg;
    if (!v8Call(info[1]->BooleanValue(info.GetIsolate()->GetCurrentContext()), canBubbleArg)
        || !v8Call(info[2]->BooleanValue(info.GetIsolate()->GetCurrentContext()), cancelableArg))
        return;
    v8::Local<v8::Value> dataArg = info[3];
    TOSTRING_VOID(V8StringResource<>, originArg, info[4]);
    TOSTRING_VOID(V8StringResource<>, lastEventIdArg, info[5]);
    DOMWindow* sourceArg = toDOMWindow(info.GetIsolate(), info[6]);
    MessagePortArray* portArray = nullptr;
    const int portArrayIndex = 7;
    if (!isUndefinedOrNull(info[portArrayIndex])) {
        portArray = new MessagePortArray;
        *portArray = toMemberNativeArray<MessagePort, V8MessagePort>(info[portArrayIndex], portArrayIndex + 1, info.GetIsolate(), exceptionState);
        if (exceptionState.throwIfNeeded())
            return;
    }
    event->initMessageEvent(typeArg, canBubbleArg, cancelableArg, ScriptValue(ScriptState::current(info.GetIsolate()), dataArg), originArg, lastEventIdArg, sourceArg, portArray);
}

} // namespace blink
