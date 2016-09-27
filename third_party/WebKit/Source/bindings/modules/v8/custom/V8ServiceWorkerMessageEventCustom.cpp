// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/modules/v8/V8ServiceWorkerMessageEvent.h"

#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/V8HiddenValue.h"

namespace blink {

void V8ServiceWorkerMessageEvent::dataAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    ServiceWorkerMessageEvent* event = V8ServiceWorkerMessageEvent::toImpl(info.Holder());
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> result = V8HiddenValue::getHiddenValue(isolate, info.Holder(), V8HiddenValue::data(isolate));

    if (!result.IsEmpty()) {
        v8SetReturnValue(info, result);
        return;
    }

    v8::Local<v8::Value> data;
    if (SerializedScriptValue* serializedValue = event->serializedData()) {
        MessagePortArray ports = event->ports();
        data = serializedValue->deserialize(isolate, &ports);
    } else {
        data = event->data().v8ValueFor(ScriptState::current(isolate));
    }
    if (data.IsEmpty())
        data = v8::Null(isolate);
    V8HiddenValue::setHiddenValue(isolate, info.Holder(), V8HiddenValue::data(isolate), data);
    v8SetReturnValue(info, data);
}

} // namespace blink
