// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8InjectedScriptHost_h
#define V8InjectedScriptHost_h

#include <v8.h>

namespace blink {

class V8InspectorImpl;

// SECURITY NOTE: Although the InjectedScriptHost is intended for use solely by the inspector,
// a reference to the InjectedScriptHost may be leaked to the page being inspected. Thus, the
// InjectedScriptHost must never implemment methods that have more power over the page than the
// page already has itself (e.g. origin restriction bypasses).

class V8InjectedScriptHost {
public:
    // We expect that debugger outlives any JS context and thus V8InjectedScriptHost (owned by JS)
    // is destroyed before inspector.
    static v8::Local<v8::Object> create(v8::Local<v8::Context>, V8InspectorImpl*);
private:
    static void internalConstructorNameCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void formatAccessorsAsProperties(const v8::FunctionCallbackInfo<v8::Value>&);
    static void subtypeCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void getInternalPropertiesCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void objectHasOwnPropertyCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void bindCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void proxyTargetValueCallback(const v8::FunctionCallbackInfo<v8::Value>&);
};

} // namespace blink

#endif // V8InjectedScriptHost_h
