/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef V8ScriptRunner_h
#define V8ScriptRunner_h

#include "bindings/core/v8/V8CacheOptions.h"
#include "core/CoreExport.h"
#include "core/fetch/AccessControlStatus.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/TextPosition.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class CachedMetadataHandler;
class Resource;
class ScriptResource;
class ScriptSourceCode;
class ExecutionContext;
class ScriptStreamer;

class CORE_EXPORT V8ScriptRunner final {
public:
    // For the following methods, the caller sites have to hold
    // a HandleScope and a ContextScope.
    static v8::MaybeLocal<v8::Script> compileScript(const ScriptSourceCode&, v8::Isolate*, AccessControlStatus = SharableCrossOrigin, V8CacheOptions = V8CacheOptionsDefault);
    static v8::MaybeLocal<v8::Script> compileScript(const String&, const String& fileName, const String& sourceMapUrl, const TextPosition&, v8::Isolate*, CachedMetadataHandler* = nullptr, AccessControlStatus = SharableCrossOrigin, V8CacheOptions = V8CacheOptionsDefault);
    // CachedMetadataHandler is set when metadata caching is supported. For
    // normal scripe resources, CachedMetadataHandler is from ScriptResource.
    // For worker script, ScriptResource is null but CachedMetadataHandler may be
    // set. When ScriptStreamer is set, ScriptResource must be set.
    static v8::MaybeLocal<v8::Script> compileScript(v8::Local<v8::String>, const String& fileName, const String& sourceMapUrl, const TextPosition&, v8::Isolate*, ScriptResource* = nullptr, ScriptStreamer* = nullptr, CachedMetadataHandler* = nullptr, AccessControlStatus = SharableCrossOrigin, V8CacheOptions = V8CacheOptionsDefault, bool isInternalScript = false);
    static v8::MaybeLocal<v8::Value> runCompiledScript(v8::Isolate*, v8::Local<v8::Script>, ExecutionContext*);
    static v8::MaybeLocal<v8::Value> compileAndRunInternalScript(v8::Local<v8::String>, v8::Isolate*, const String& = String(), const TextPosition& = TextPosition());
    static v8::MaybeLocal<v8::Value> runCompiledInternalScript(v8::Isolate*, v8::Local<v8::Script>);
    static v8::MaybeLocal<v8::Value> callInternalFunction(v8::Local<v8::Function>, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> info[], v8::Isolate*);
    static v8::MaybeLocal<v8::Value> callFunction(v8::Local<v8::Function>, ExecutionContext*, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> info[], v8::Isolate*);
    static v8::MaybeLocal<v8::Object> instantiateObject(v8::Isolate*, v8::Local<v8::ObjectTemplate>);
    static v8::MaybeLocal<v8::Object> instantiateObject(v8::Isolate*, v8::Local<v8::Function>, int argc = 0, v8::Local<v8::Value> argv[] = 0);
    static v8::MaybeLocal<v8::Object> instantiateObjectInDocument(v8::Isolate*, v8::Local<v8::Function>, ExecutionContext*, int argc = 0, v8::Local<v8::Value> argv[] = 0);

    static unsigned tagForParserCache(CachedMetadataHandler*);
    static unsigned tagForCodeCache(CachedMetadataHandler*);
};

} // namespace blink

#endif // V8ScriptRunner_h
