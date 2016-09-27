// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PrivateScriptRunner_h
#define PrivateScriptRunner_h

#include "bindings/core/v8/ExceptionState.h"
#include "core/CoreExport.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class Document;
class ScriptState;

class CORE_EXPORT PrivateScriptRunner {
public:
    static v8::Local<v8::Value> installClassIfNeeded(Document*, String className);
    static v8::Local<v8::Value> runDOMAttributeGetter(ScriptState*, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder);
    static bool runDOMAttributeSetter(ScriptState*, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder, v8::Local<v8::Value> v8Value);
    static v8::Local<v8::Value> runDOMMethod(ScriptState*, ScriptState* scriptStateInUserScript, const char* className, const char* methodName, v8::Local<v8::Value> holder, int argc, v8::Local<v8::Value> argv[]);
};

} // namespace blink

#endif // V8PrivateScriptRunner_h
