/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef V8ThrowException_h
#define V8ThrowException_h

#include "core/CoreExport.h"
#include "wtf/text/WTFString.h"
#include <v8.h>

namespace blink {

class CORE_EXPORT V8ThrowException {
public:

    static v8::Local<v8::Value> createDOMException(v8::Isolate* isolate, int ec, const String& message, const v8::Local<v8::Object>& creationContext)
    {
        return createDOMException(isolate, ec, message, String(), creationContext);
    }
    static v8::Local<v8::Value> createDOMException(v8::Isolate*, int, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Local<v8::Object>& creationContext);

    static v8::Local<v8::Value> throwDOMException(int ec, const String& message, const v8::Local<v8::Object>& creationContext, v8::Isolate* isolate)
    {
        return throwDOMException(ec, message, String(), creationContext, isolate);
    }
    static v8::Local<v8::Value> throwDOMException(int, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Local<v8::Object>& creationContext, v8::Isolate*);

    static v8::Local<v8::Value> throwException(v8::Local<v8::Value>, v8::Isolate*);

    static v8::Local<v8::Value> createGeneralError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> throwGeneralError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> createTypeError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> throwTypeError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> createRangeError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> throwRangeError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> createSyntaxError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> throwSyntaxError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> createReferenceError(v8::Isolate*, const String&);
    static v8::Local<v8::Value> throwReferenceError(v8::Isolate*, const String&);
};

} // namespace blink

#endif // V8ThrowException_h
