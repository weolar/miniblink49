/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "bindings/core/v8/ArrayValue.h"
#include "bindings/core/v8/DictionaryHelperForBindings.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMError.h"
#include "bindings/core/v8/V8Element.h"
#include "bindings/core/v8/V8EventTarget.h"
#include "bindings/core/v8/V8MediaKeyError.h"
#include "bindings/core/v8/V8MessagePort.h"
#include "bindings/core/v8/V8TextTrack.h"
#include "bindings/core/v8/V8Uint8Array.h"
#include "bindings/core/v8/V8VoidCallback.h"
#include "bindings/core/v8/V8Window.h"
#include "core/html/track/TrackBase.h"
#include "wtf/MathExtras.h"

namespace blink {

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, v8::Local<v8::Value>& value)
{
    return dictionary.get(key, value);
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, Dictionary& value)
{
    return dictionary.get(key, value);
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, bool& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    return v8Call(v8Value->BooleanValue(dictionary.v8Context()), value);
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, int32_t& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    return v8Call(v8Value->Int32Value(dictionary.v8Context()), value);
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, double& value, bool& hasValue)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value)) {
        hasValue = false;
        return false;
    }

    hasValue = true;
    return v8Call(v8Value->NumberValue(dictionary.v8Context()), value);
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, double& value)
{
    bool unused;
    return DictionaryHelper::get(dictionary, key, value, unused);
}

template<typename StringType>
bool getStringType(const Dictionary& dictionary, const String& key, StringType& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    V8StringResource<> stringValue(v8Value);
    if (!stringValue.prepare())
        return false;
    value = stringValue;
    return true;
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, String& value)
{
    return getStringType(dictionary, key, value);
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, AtomicString& value)
{
    return getStringType(dictionary, key, value);
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, ScriptValue& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    value = ScriptValue(ScriptState::current(dictionary.isolate()), v8Value);
    return true;
}

template<typename NumericType>
bool getNumericType(const Dictionary& dictionary, const String& key, NumericType& value)
{
    int32_t int32Value;
    if (!DictionaryHelper::get(dictionary, key, int32Value))
        return false;
    value = static_cast<NumericType>(int32Value);
    return true;
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, short& value)
{
    return getNumericType<short>(dictionary, key, value);
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, unsigned short& value)
{
    return getNumericType<unsigned short>(dictionary, key, value);
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, unsigned& value)
{
    return getNumericType<unsigned>(dictionary, key, value);
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, unsigned long& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    int64_t int64Value;
    if (!v8Call(v8Value->IntegerValue(dictionary.v8Context()), int64Value))
        return false;
    value = int64Value;
    return true;
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, unsigned long long& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    double doubleValue;
    if (!v8Call(v8Value->NumberValue(dictionary.v8Context()), doubleValue))
        return false;
    doubleToInteger(doubleValue, value);
    return true;
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, RefPtrWillBeMember<DOMWindow>& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    // We need to handle a DOMWindow specially, because a DOMWindow wrapper
    // exists on a prototype chain of v8Value.
    value = toDOMWindow(dictionary.isolate(), v8Value);
    return true;
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, RefPtrWillBeMember<TrackBase>& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    TrackBase* source = 0;
    if (v8Value->IsObject()) {
        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::Cast(v8Value);

        // FIXME: this will need to be changed so it can also return an AudioTrack or a VideoTrack once
        // we add them.
        v8::Local<v8::Object> track = V8TextTrack::findInstanceInPrototypeChain(wrapper, dictionary.isolate());
        if (!track.IsEmpty())
            source = V8TextTrack::toImpl(track);
    }
    value = source;
    return true;
}

template <>
bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, RefPtrWillBeMember<EventTarget>& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    value = nullptr;
    // We need to handle a DOMWindow specially, because a DOMWindow wrapper
    // exists on a prototype chain of v8Value.
    if (v8Value->IsObject()) {
        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::Cast(v8Value);
        v8::Local<v8::Object> window = V8Window::findInstanceInPrototypeChain(wrapper, dictionary.isolate());
        if (!window.IsEmpty()) {
            value = toWrapperTypeInfo(window)->toEventTarget(window);
            return true;
        }
    }

    if (V8DOMWrapper::isWrapper(dictionary.isolate(), v8Value)) {
        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::Cast(v8Value);
        value = toWrapperTypeInfo(wrapper)->toEventTarget(wrapper);
    }
    return true;
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, Vector<String>& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    if (!v8Value->IsArray())
        return false;

    v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::Cast(v8Value);
    for (size_t i = 0; i < v8Array->Length(); ++i) {
        v8::Local<v8::Value> indexedValue;
        if (!v8Array->Get(dictionary.v8Context(), v8::Uint32::New(dictionary.isolate(), i)).ToLocal(&indexedValue))
            return false;
        TOSTRING_DEFAULT(V8StringResource<>, stringValue, indexedValue, false);
        value.append(stringValue);
    }

    return true;
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, Vector<Vector<String>>& value, ExceptionState& exceptionState)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    if (!v8Value->IsArray())
        return false;

    v8::Local<v8::Array> v8Array = v8::Local<v8::Array>::Cast(v8Value);
    for (size_t i = 0; i < v8Array->Length(); ++i) {
        v8::Local<v8::Value> v8IndexedValue;
        if (!v8Array->Get(dictionary.v8Context(), v8::Uint32::New(dictionary.isolate(), i)).ToLocal(&v8IndexedValue))
            return false;
        Vector<String> indexedValue = toImplArray<Vector<String>>(v8IndexedValue, i, dictionary.isolate(), exceptionState);
        if (exceptionState.hadException())
            return false;
        value.append(indexedValue);
    }

    return true;
}

template <>
CORE_EXPORT bool DictionaryHelper::get(const Dictionary& dictionary, const String& key, ArrayValue& value)
{
    v8::Local<v8::Value> v8Value;
    if (!dictionary.get(key, v8Value))
        return false;

    if (!v8Value->IsArray())
        return false;

    ASSERT(dictionary.isolate());
    ASSERT(dictionary.isolate() == v8::Isolate::GetCurrent());
    value = ArrayValue(v8::Local<v8::Array>::Cast(v8Value), dictionary.isolate());
    return true;
}

template CORE_EXPORT bool DictionaryHelper::get(const Dictionary&, const String& key, RefPtr<DOMUint8Array>& value);

template <typename T>
struct IntegralTypeTraits {
};

template <>
struct IntegralTypeTraits<uint8_t> {
    static inline uint8_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toUInt8(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "UInt8"; }
};

template <>
struct IntegralTypeTraits<int8_t> {
    static inline int8_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toInt8(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "Int8"; }
};

template <>
struct IntegralTypeTraits<unsigned short> {
    static inline uint16_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toUInt16(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "UInt16"; }
};

template <>
struct IntegralTypeTraits<short> {
    static inline int16_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toInt16(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "Int16"; }
};

template <>
struct IntegralTypeTraits<unsigned> {
    static inline uint32_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toUInt32(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "UInt32"; }
};

template <>
struct IntegralTypeTraits<unsigned long> {
    static inline uint32_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toUInt32(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "UInt32"; }
};

template <>
struct IntegralTypeTraits<int> {
    static inline int32_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toInt32(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "Int32"; }
};

template <>
struct IntegralTypeTraits<long> {
    static inline int32_t toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toInt32(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "Int32"; }
};

template <>
struct IntegralTypeTraits<unsigned long long> {
    static inline unsigned long long toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toUInt64(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "UInt64"; }
};

template <>
struct IntegralTypeTraits<long long> {
    static inline long long toIntegral(v8::Isolate* isolate, v8::Local<v8::Value> value, IntegerConversionConfiguration configuration, ExceptionState& exceptionState)
    {
        return toInt64(isolate, value, configuration, exceptionState);
    }
    static const String typeName() { return "Int64"; }
};

} // namespace blink
