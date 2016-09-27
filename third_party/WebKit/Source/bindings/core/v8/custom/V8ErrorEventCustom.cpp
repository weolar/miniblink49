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
#include "bindings/core/v8/V8ErrorEvent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "bindings/core/v8/V8Event.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "core/dom/ContextFeatures.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

static void setHiddenValueAndReturnValue(const v8::FunctionCallbackInfo<v8::Value>& info, v8::Local<v8::Value> error)
{
    V8HiddenValue::setHiddenValue(info.GetIsolate(), info.Holder(), V8HiddenValue::error(info.GetIsolate()), error);
    v8SetReturnValue(info, error);
}

void V8ErrorEvent::errorAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> cachedError = V8HiddenValue::getHiddenValue(isolate, info.Holder(), V8HiddenValue::error(isolate));
    if (!cachedError.IsEmpty()) {
        v8SetReturnValue(info, cachedError);
        return;
    }

    ErrorEvent* event = V8ErrorEvent::toImpl(info.Holder());
    ScriptValue error = event->error(ScriptState::current(isolate));
    if (!error.isEmpty())
        setHiddenValueAndReturnValue(info, error.v8Value());
    else
        setHiddenValueAndReturnValue(info, v8::Null(isolate));
}

} // namespace blink
