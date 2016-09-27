// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8BindingForTesting_h
#define V8BindingForTesting_h

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptState.h"
#include "wtf/Forward.h"

#include <v8.h>

namespace blink {

class ScriptStateForTesting : public ScriptState {
public:
    static PassRefPtr<ScriptStateForTesting> create(v8::Local<v8::Context>, PassRefPtr<DOMWrapperWorld>);
    ExecutionContext* executionContext() const override;
    void setExecutionContext(ExecutionContext*) override;
private:
    ScriptStateForTesting(v8::Local<v8::Context>, PassRefPtr<DOMWrapperWorld>);
    ExecutionContext* m_executionContext;
};

class V8TestingScope {
public:
    explicit V8TestingScope(v8::Isolate*);
    ScriptState* scriptState() const;
    v8::Isolate* isolate() const;
    v8::Local<v8::Context> context() const;
    ~V8TestingScope();

private:
    v8::HandleScope m_handleScope;
    v8::Context::Scope m_contextScope;
    RefPtr<ScriptState> m_scriptState;
};

} // namespace blink

#endif // V8BindingForTesting_h
