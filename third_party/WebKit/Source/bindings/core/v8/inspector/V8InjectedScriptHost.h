// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8InjectedScriptHost_h
#define V8InjectedScriptHost_h

#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include <v8.h>

namespace blink {

class InjectedScriptHost;

class V8InjectedScriptHost {
public:
    static v8::Local<v8::Object> wrap(v8::Local<v8::FunctionTemplate> constructorTemplate, v8::Local<v8::Context>, PassRefPtrWillBeRawPtr<InjectedScriptHost>);
    static InjectedScriptHost* unwrap(v8::Local<v8::Object>);
    static v8::Local<v8::FunctionTemplate> createWrapperTemplate(v8::Isolate*);

    static void clearConsoleMessagesCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void inspectCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void inspectedObjectCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void internalConstructorNameCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void isDOMWrapperCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void isHTMLAllCollectionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void isTypedArrayCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void subtypeCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void functionDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void generatorObjectDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void collectionEntriesCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void getInternalPropertiesCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void getEventListenersCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void evalCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void evaluateWithExceptionDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void debugFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void undebugFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void monitorFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void unmonitorFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void callFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void suppressWarningsAndCallFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void setNonEnumPropertyCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void setFunctionVariableValueCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void bindCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void objectForIdCallback(const v8::FunctionCallbackInfo<v8::Value>&);
    static void idToObjectGroupNameCallback(const v8::FunctionCallbackInfo<v8::Value>&);
};

} // namespace blink

#endif // V8InjectedScriptHost_h
