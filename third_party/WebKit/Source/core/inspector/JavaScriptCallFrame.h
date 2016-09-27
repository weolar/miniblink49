/*
 * Copyright (c) 2010, Google Inc. All rights reserved.
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

#ifndef JavaScriptCallFrame_h
#define JavaScriptCallFrame_h


#include "bindings/core/v8/ScopedPersistent.h"
#include "bindings/core/v8/ScriptState.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class JavaScriptCallFrame : public RefCounted<JavaScriptCallFrame> {
public:
    static PassRefPtr<JavaScriptCallFrame> create(v8::Local<v8::Context> debuggerContext, v8::Local<v8::Object> callFrame)
    {
        return adoptRef(new JavaScriptCallFrame(debuggerContext, callFrame));
    }
    ~JavaScriptCallFrame();

    JavaScriptCallFrame* caller();

    int sourceID() const;
    int line() const;
    int column() const;
    String scriptName() const;
    String functionName() const;
    int functionLine() const;
    int functionColumn() const;

    v8::Local<v8::Value> scopeChain() const;
    int scopeType(int scopeIndex) const;
    v8::Local<v8::Value> thisObject() const;
    String stepInPositions() const;
    bool isAtReturn() const;
    v8::Local<v8::Value> returnValue() const;

    v8::Local<v8::Value> evaluateWithExceptionDetails(v8::Local<v8::Value> expression, v8::Local<v8::Value> scopeExtension);
    v8::MaybeLocal<v8::Value> restart();
    v8::MaybeLocal<v8::Value> setVariableValue(int scopeNumber, v8::Local<v8::Value> variableName, v8::Local<v8::Value> newValue);

    static v8::Local<v8::Object> createExceptionDetails(v8::Isolate*, v8::Local<v8::Message>);

    // FIXME: store this template in per isolate data
    void setWrapperTemplate(v8::Local<v8::FunctionTemplate> wrapperTemplate, v8::Isolate* isolate) { m_wrapperTemplate.Reset(isolate, wrapperTemplate); }
    v8::Local<v8::FunctionTemplate> wrapperTemplate(v8::Isolate* isolate) { return v8::Local<v8::FunctionTemplate>::New(isolate, m_wrapperTemplate); }

private:
    JavaScriptCallFrame(v8::Local<v8::Context> debuggerContext, v8::Local<v8::Object> callFrame);

    int callV8FunctionReturnInt(const char* name) const;
    String callV8FunctionReturnString(const char* name) const;

    v8::Isolate* m_isolate;
    RefPtr<JavaScriptCallFrame> m_caller;
    ScopedPersistent<v8::Context> m_debuggerContext;
    ScopedPersistent<v8::Object> m_callFrame;
    v8::Global<v8::FunctionTemplate> m_wrapperTemplate;
};

} // namespace blink

#endif // JavaScriptCallFrame_h
