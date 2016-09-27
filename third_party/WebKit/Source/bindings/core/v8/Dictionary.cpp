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
#include "bindings/core/v8/Dictionary.h"

#include "bindings/core/v8/ArrayValue.h"
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
#include "bindings/core/v8/V8VoidCallback.h"
#include "bindings/core/v8/V8Window.h"
#include "core/html/track/TrackBase.h"
#include "wtf/MathExtras.h"

namespace blink {

static ExceptionState& emptyExceptionState()
{
    AtomicallyInitializedStaticReference(WTF::ThreadSpecific<NonThrowableExceptionState>, exceptionState, new ThreadSpecific<NonThrowableExceptionState>);
    return *exceptionState;
}

Dictionary::Dictionary()
    : m_isolate(0)
    , m_exceptionState(&emptyExceptionState())
{
}

Dictionary::Dictionary(const v8::Local<v8::Value>& options, v8::Isolate* isolate, ExceptionState& exceptionState)
    : m_options(options)
    , m_isolate(isolate)
    , m_exceptionState(&exceptionState)
{
    ASSERT(m_isolate);
    ASSERT(m_exceptionState);
#if ENABLE(ASSERT)
    m_exceptionState->onStackObjectChecker().add(this);
#endif
}

Dictionary::~Dictionary()
{
#if ENABLE(ASSERT)
    if (m_exceptionState)
        m_exceptionState->onStackObjectChecker().remove(this);
#endif
}

Dictionary& Dictionary::operator=(const Dictionary& optionsObject)
{
    m_options = optionsObject.m_options;
    m_isolate = optionsObject.m_isolate;
#if ENABLE(ASSERT)
    if (m_exceptionState)
        m_exceptionState->onStackObjectChecker().remove(this);
#endif
    m_exceptionState = optionsObject.m_exceptionState;
#if ENABLE(ASSERT)
    if (m_exceptionState)
        m_exceptionState->onStackObjectChecker().add(this);
#endif
    return *this;
}

bool Dictionary::isObject() const
{
    return !isUndefinedOrNull() && m_options->IsObject();
}

bool Dictionary::isUndefinedOrNull() const
{
    if (m_options.IsEmpty())
        return true;
    return blink::isUndefinedOrNull(m_options);
}

bool Dictionary::hasProperty(const String& key) const
{
    v8::Local<v8::Object> object;
    if (!toObject(object))
        return false;

    ASSERT(m_isolate);
    ASSERT(m_isolate == v8::Isolate::GetCurrent());
    ASSERT(m_exceptionState);
    v8::Local<v8::String> v8Key = v8String(m_isolate, key);
    return v8CallBoolean(object->Has(v8Context(), v8Key));
}

bool Dictionary::getKey(const String& key, v8::Local<v8::Value>& value) const
{
    v8::Local<v8::Object> object;
    if (!toObject(object))
        return false;

    ASSERT(m_isolate);
    ASSERT(m_isolate == v8::Isolate::GetCurrent());
    ASSERT(m_exceptionState);
    v8::Local<v8::String> v8Key = v8String(m_isolate, key);
    if (!v8CallBoolean(object->Has(v8Context(), v8Key)))
        return false;
    return object->Get(v8Context(), v8Key).ToLocal(&value);
}

bool Dictionary::get(const String& key, v8::Local<v8::Value>& value) const
{
    return getKey(key, value);
}

bool Dictionary::get(const String& key, Dictionary& value) const
{
    v8::Local<v8::Value> v8Value;
    if (!getKey(key, v8Value))
        return false;

    if (v8Value->IsObject()) {
        ASSERT(m_isolate);
        ASSERT(m_isolate == v8::Isolate::GetCurrent());
        value = Dictionary(v8Value, m_isolate, *m_exceptionState);
    }

    return true;
}

static inline bool propertyKey(v8::Local<v8::Context> v8Context, v8::Local<v8::Array> properties, uint32_t index, v8::Local<v8::String>& key)
{
    v8::Local<v8::Value> property;
    if (!properties->Get(v8Context, index).ToLocal(&property))
        return false;
    return property->ToString(v8Context).ToLocal(&key);
}

bool Dictionary::getOwnPropertiesAsStringHashMap(HashMap<String, String>& hashMap) const
{
    v8::Local<v8::Object> object;
    if (!toObject(object))
        return false;

    v8::Local<v8::Array> properties;
    if (!object->GetOwnPropertyNames(v8Context()).ToLocal(&properties))
        return false;
    for (uint32_t i = 0; i < properties->Length(); ++i) {
        v8::Local<v8::String> key;
        if (!propertyKey(v8Context(), properties, i, key))
            continue;
        if (!v8CallBoolean(object->Has(v8Context(), key)))
            continue;

        v8::Local<v8::Value> value;
        if (!object->Get(v8Context(), key).ToLocal(&value))
            continue;
        TOSTRING_DEFAULT(V8StringResource<>, stringKey, key, false);
        TOSTRING_DEFAULT(V8StringResource<>, stringValue, value, false);
        if (!static_cast<const String&>(stringKey).isEmpty())
            hashMap.set(stringKey, stringValue);
    }

    return true;
}

bool Dictionary::getPropertyNames(Vector<String>& names) const
{
    v8::Local<v8::Object> object;
    if (!toObject(object))
        return false;

    v8::Local<v8::Array> properties;
    if (!object->GetPropertyNames(v8Context()).ToLocal(&properties))
        return false;
    for (uint32_t i = 0; i < properties->Length(); ++i) {
        v8::Local<v8::String> key;
        if (!propertyKey(v8Context(), properties, i, key))
            continue;
        if (!v8CallBoolean(object->Has(v8Context(), key)))
            continue;
        TOSTRING_DEFAULT(V8StringResource<>, stringKey, key, false);
        names.append(stringKey);
    }

    return true;
}

bool Dictionary::toObject(v8::Local<v8::Object>& object) const
{
    return !isUndefinedOrNull() && m_options->ToObject(v8Context()).ToLocal(&object);
}

} // namespace blink
