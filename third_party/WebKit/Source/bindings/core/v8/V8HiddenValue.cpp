// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8HiddenValue.h"

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/V8Binding.h"

namespace blink {

#define V8_DEFINE_METHOD(name) \
v8::Local<v8::String> V8HiddenValue::name(v8::Isolate* isolate)    \
{ \
    V8HiddenValue* hiddenValue = V8PerIsolateData::from(isolate)->hiddenValue(); \
    if (hiddenValue->m_##name.isEmpty()) { \
        hiddenValue->m_##name.set(isolate, v8AtomicString(isolate, #name)); \
    } \
    return hiddenValue->m_##name.newLocal(isolate); \
}

V8_HIDDEN_VALUES(V8_DEFINE_METHOD);

v8::Local<v8::Value> V8HiddenValue::getHiddenValue(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::String> key)
{	//zero
#if V8_MAJOR_VERSION > 5 || (V8_MAJOR_VERSION == 5 && V8_MINOR_VERSION == 7)
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Private> privateKey =
        v8::Private::ForApi(isolate, key);
    v8::Local<v8::Value> value;
    // Callsites interpret an empty handle has absence of a result.
    if (!v8CallBoolean(object->HasPrivate(context, privateKey)))
        return v8::Local<v8::Value>();
    if (object->GetPrivate(context, privateKey).ToLocal(&value))
        return value;
    return v8::Local<v8::Value>();
#else
    return object->GetHiddenValue(key);
#endif
}

bool V8HiddenValue::setHiddenValue(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::String> key, v8::Local<v8::Value> value)
{
	//zero
#if V8_MAJOR_VERSION > 5 || (V8_MAJOR_VERSION == 5 && V8_MINOR_VERSION == 7)
    if (UNLIKELY(value.IsEmpty()))
        return false;
    return v8CallBoolean(object->SetPrivate(isolate->GetCurrentContext(), v8::Private::ForApi(isolate, key), value));
#else
    return object->SetHiddenValue(key, value);
#endif
}

bool V8HiddenValue::deleteHiddenValue(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::String> key)
{
	//zero
#if V8_MAJOR_VERSION > 5 || (V8_MAJOR_VERSION == 5 && V8_MINOR_VERSION == 7)
    return v8CallBoolean(object->SetPrivate(
        isolate->GetCurrentContext(), v8::Private::ForApi(isolate, key),
        v8::Undefined(isolate)));
#else
    return object->DeleteHiddenValue(key);
#endif
}

v8::Local<v8::Value> V8HiddenValue::getHiddenValueFromMainWorldWrapper(v8::Isolate* isolate, ScriptWrappable* wrappable, v8::Local<v8::String> key)
{
    v8::Local<v8::Object> wrapper = wrappable->newLocalWrapper(isolate);
    return wrapper.IsEmpty() ? v8::Local<v8::Value>() : getHiddenValue(isolate, wrapper, key);
}

} // namespace blink
