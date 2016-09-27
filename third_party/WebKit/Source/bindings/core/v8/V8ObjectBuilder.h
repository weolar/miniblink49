// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8ObjectBuilder_h
#define V8ObjectBuilder_h

#include "bindings/core/v8/ToV8.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class ScriptState;
class ScriptValue;

class CORE_EXPORT V8ObjectBuilder final {
    STACK_ALLOCATED();
public:
    explicit V8ObjectBuilder(ScriptState*);

    ScriptState* scriptState() const { return m_scriptState.get(); }

    V8ObjectBuilder& add(const String& name, const V8ObjectBuilder&);

    V8ObjectBuilder& addNull(const String& name);
    V8ObjectBuilder& addBoolean(const String& name, bool value);
    V8ObjectBuilder& addNumber(const String& name, double value);
    V8ObjectBuilder& addString(const String& name, const String& value);

    template <typename T>
    V8ObjectBuilder& add(const String& name, const T& value)
    {
        addInternal(name, v8::Local<v8::Value>(toV8(value, m_scriptState->context()->Global(), m_scriptState->isolate())));
        return *this;
    }

    ScriptValue scriptValue() const;
    v8::Local<v8::Object> v8Value() const { return m_object; }

private:
    void addInternal(const String& name, v8::Local<v8::Value>);

    RefPtr<ScriptState> m_scriptState;
    v8::Local<v8::Object> m_object;
};

} // namespace blink

#endif // V8ObjectBuilder_h
