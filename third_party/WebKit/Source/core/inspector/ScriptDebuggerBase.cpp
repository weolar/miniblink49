// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/ScriptDebuggerBase.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "public/platform/Platform.h"
#include "public/platform/WebData.h"

namespace blink {

ScriptDebuggerBase::ScriptDebuggerBase(v8::Isolate* isolate, PassOwnPtrWillBeRawPtr<V8Debugger> debugger)
    : m_isolate(isolate)
    , m_debugger(debugger)
{
}

ScriptDebuggerBase::~ScriptDebuggerBase()
{
}

DEFINE_TRACE(ScriptDebuggerBase)
{
    visitor->trace(m_debugger);
    V8Debugger::Client::trace(visitor);
}

v8::Local<v8::Object> ScriptDebuggerBase::compileDebuggerScript()
{
    const WebData& debuggerScriptSourceResource = Platform::current()->loadResource("DebuggerScriptSource.js");
    v8::Local<v8::String> source = v8String(m_isolate, String(debuggerScriptSourceResource.data(), debuggerScriptSourceResource.size()));
    v8::Local<v8::Value> value;
    if (!V8ScriptRunner::compileAndRunInternalScript(source, m_isolate).ToLocal(&value))
        return v8::Local<v8::Object>();
    ASSERT(value->IsObject());
    return value.As<v8::Object>();
}

}
