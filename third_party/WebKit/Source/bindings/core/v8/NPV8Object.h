/*
 * Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
 * Copyright (C) 2014 Opera Software ASA. All rights reserved.
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
#ifndef NPV8Object_h
#define NPV8Object_h

#include "bindings/core/v8/V8DOMWrapper.h"
#include "core/CoreExport.h"

// Chromium uses npruntime.h from the Chromium source repository under
// third_party/npapi/bindings.
#include <bindings/npruntime.h>
#include <v8.h>

namespace blink {

class LocalDOMWindow;
class ScriptWrappable;

static const int npObjectInternalFieldCount = v8DefaultWrapperInternalFieldCount + 0;

const WrapperTypeInfo* npObjectTypeInfo();

// A V8NPObject is a NPObject which carries additional V8-specific information.
// It is created with npCreateV8ScriptObject() and deallocated via the deallocate
// method in the same way as other NPObjects.
struct V8NPObject {
    WTF_MAKE_NONCOPYABLE(V8NPObject);
public:
    NPObject object;
    v8::Persistent<v8::Object> v8Object;
    LocalDOMWindow* rootObject;
};

struct PrivateIdentifier {
    union {
        const NPUTF8* string;
        int32_t number;
    } value;
    bool isString;
};

CORE_EXPORT NPObject* npCreateV8ScriptObject(v8::Isolate*, NPP, v8::Local<v8::Object>, LocalDOMWindow*);

NPObject* v8ObjectToNPObject(v8::Local<v8::Object>);

bool isWrappedNPObject(v8::Local<v8::Object>);

CORE_EXPORT V8NPObject* npObjectToV8NPObject(NPObject*);

ScriptWrappable* npObjectToScriptWrappable(NPObject*);

void disposeUnderlyingV8Object(v8::Isolate*, NPObject*);

} // namespace blink

#endif // NPV8Object_h
