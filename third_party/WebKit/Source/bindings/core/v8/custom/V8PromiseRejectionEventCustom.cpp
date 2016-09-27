// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8PromiseRejectionEvent.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/V8Binding.h"

namespace blink {

void V8PromiseRejectionEvent::promiseAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    PromiseRejectionEvent* event = V8PromiseRejectionEvent::toImpl(info.Holder());
    ScriptPromise promise = event->promise(ScriptState::current(isolate));
    if (promise.isEmpty()) {
        v8SetReturnValue(info, v8::Null(isolate));
        return;
    }

    v8SetReturnValue(info, promise.v8Value());
}

void V8PromiseRejectionEvent::visitDOMWrapper(v8::Isolate* isolate, ScriptWrappable* scriptWrappable, const v8::Persistent<v8::Object>& wrapper)
{
    PromiseRejectionEvent* event = scriptWrappable->toImpl<PromiseRejectionEvent>();
    event->setWrapperReference(isolate, wrapper);
}

} // namespace blink
