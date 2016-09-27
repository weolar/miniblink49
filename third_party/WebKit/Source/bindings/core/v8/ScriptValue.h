/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
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

#ifndef ScriptValue_h
#define ScriptValue_h

#include "bindings/core/v8/NativeValueTraits.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/SharedPersistent.h"
#include "core/CoreExport.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class JSONValue;

class CORE_EXPORT ScriptValue final {
public:
    template<typename T>
    static ScriptValue from(ScriptState* scriptState, T value)
    {
        return ScriptValue(scriptState, toV8(value, scriptState->context()->Global(), scriptState->isolate()));
    }

    template<typename T, typename... Arguments>
    static inline T to(v8::Isolate* isolate, v8::Local<v8::Value> value, ExceptionState& exceptionState, Arguments const&... arguments)
    {
        return NativeValueTraits<T>::nativeValue(isolate, value, exceptionState, arguments...);
    }

    template<typename T, typename... Arguments>
    static inline T to(v8::Isolate* isolate, const ScriptValue& value, ExceptionState& exceptionState, Arguments const&... arguments)
    {
        return to<T>(isolate, value.v8Value(), exceptionState, arguments...);
    }

    ScriptValue() { }

    ScriptValue(ScriptState* scriptState, v8::Local<v8::Value> value)
        : m_scriptState(scriptState)
        , m_value(value.IsEmpty() ? nullptr : SharedPersistent<v8::Value>::create(value, scriptState->isolate()))
    {
        ASSERT(isEmpty() || m_scriptState);
    }

    template <typename T>
    ScriptValue(ScriptState* scriptState, v8::MaybeLocal<T> value)
        : m_scriptState(scriptState)
        , m_value(value.IsEmpty() ? nullptr : SharedPersistent<v8::Value>::create(value.ToLocalChecked(), scriptState->isolate()))
    {
        ASSERT(isEmpty() || m_scriptState);
    }

    ScriptValue(const ScriptValue& value)
        : m_scriptState(value.m_scriptState)
        , m_value(value.m_value)
    {
        ASSERT(isEmpty() || m_scriptState);
    }

    ScriptState* scriptState() const
    {
        return m_scriptState.get();
    }

    v8::Isolate* isolate() const
    {
        return m_scriptState ? m_scriptState->isolate() : v8::Isolate::GetCurrent();
    }

    v8::Local<v8::Context> context() const
    {
        ASSERT(m_scriptState.get());
        return m_scriptState->context();
    }

    ScriptValue& operator=(const ScriptValue& value)
    {
        if (this != &value) {
            m_scriptState = value.m_scriptState;
            m_value = value.m_value;
        }
        return *this;
    }

    bool operator==(const ScriptValue& value) const
    {
        if (isEmpty())
            return value.isEmpty();
        if (value.isEmpty())
            return false;
        return *m_value == *value.m_value;
    }

    bool operator!=(const ScriptValue& value) const
    {
        return !operator==(value);
    }

    // This creates a new local Handle; Don't use this in performance-sensitive places.
    bool isFunction() const
    {
        ASSERT(!isEmpty());
        v8::Local<v8::Value> value = v8Value();
        return !value.IsEmpty() && value->IsFunction();
    }

    // This creates a new local Handle; Don't use this in performance-sensitive places.
    bool isNull() const
    {
        ASSERT(!isEmpty());
        v8::Local<v8::Value> value = v8Value();
        return !value.IsEmpty() && value->IsNull();
    }

    // This creates a new local Handle; Don't use this in performance-sensitive places.
    bool isUndefined() const
    {
        ASSERT(!isEmpty());
        v8::Local<v8::Value> value = v8Value();
        return !value.IsEmpty() && value->IsUndefined();
    }

    // This creates a new local Handle; Don't use this in performance-sensitive places.
    bool isObject() const
    {
        ASSERT(!isEmpty());
        v8::Local<v8::Value> value = v8Value();
        return !value.IsEmpty() && value->IsObject();
    }

    bool isEmpty() const
    {
        return !m_value.get() || m_value->isEmpty();
    }

    void clear()
    {
        m_value = nullptr;
    }

    v8::Local<v8::Value> v8Value() const;
    // TODO(bashi): Remove v8ValueUnsafe().
    v8::Local<v8::Value> v8ValueUnsafe() const;
    // Returns v8Value() if a given ScriptState is the same as the
    // ScriptState which is associated with this ScriptValue. Otherwise
    // this "clones" the v8 value and returns it.
    v8::Local<v8::Value> v8ValueFor(ScriptState*);

    bool toString(String&) const;

    void setReference(const v8::Persistent<v8::Object>& parent, v8::Isolate* isolate)
    {
        m_value->setReference(parent, isolate);
    }

    static ScriptValue createNull(ScriptState*);

private:
    RefPtr<ScriptState> m_scriptState;
    RefPtr<SharedPersistent<v8::Value>> m_value;
};

} // namespace blink

#endif // ScriptValue_h
