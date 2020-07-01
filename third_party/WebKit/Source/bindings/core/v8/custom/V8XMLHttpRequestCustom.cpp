/*
 * Copyright (C) 2008, 2009, 2010 Google Inc. All rights reserved.
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

#include "config.h"
#include "bindings/core/v8/V8XMLHttpRequest.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8Document.h"
#include "bindings/core/v8/V8FormData.h"
#include "bindings/core/v8/V8HTMLDocument.h"
#include "bindings/core/v8/V8Stream.h"
#include "core/dom/Document.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/streams/Stream.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/xmlhttprequest/XMLHttpRequest.h"
#include <v8.h>

namespace blink {

void V8XMLHttpRequest::responseTextAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    XMLHttpRequest* xmlHttpRequest = V8XMLHttpRequest::toImpl(info.Holder());
    ExceptionState exceptionState(ExceptionState::GetterContext, "responseText", "XMLHttpRequest", info.Holder(), info.GetIsolate());
    ScriptString text = xmlHttpRequest->responseText(exceptionState);
    if (exceptionState.throwIfNeeded())
        return;
    if (text.isEmpty()) {
        v8SetReturnValueString(info, emptyString(), info.GetIsolate());
        return;
    }
    v8SetReturnValue(info, text.v8Value());
}

void V8XMLHttpRequest::responseAttributeGetterCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    XMLHttpRequest* xmlHttpRequest = V8XMLHttpRequest::toImpl(info.Holder());

    switch (xmlHttpRequest->responseTypeCode()) {
    case XMLHttpRequest::ResponseTypeDefault:
    case XMLHttpRequest::ResponseTypeText:
        responseTextAttributeGetterCustom(info);
        return;

    case XMLHttpRequest::ResponseTypeJSON:
        {
            v8::Isolate* isolate = info.GetIsolate();

            ScriptString jsonSource = xmlHttpRequest->responseJSONSource();
            if (jsonSource.isEmpty()) {
                v8SetReturnValue(info, v8::Null(isolate));
                return;
            }

            // Catch syntax error. Swallows an exception (when thrown) as the
            // spec says. https://xhr.spec.whatwg.org/#response-body
            v8::TryCatch exceptionCatcher(isolate);
            v8::Local<v8::Value> json;
            if (v8Call(v8::JSON::Parse(isolate, jsonSource.v8Value()), json, exceptionCatcher))
                v8SetReturnValue(info, json);
            else
                v8SetReturnValue(info, v8::Null(isolate));
            return;
        }

    case XMLHttpRequest::ResponseTypeDocument:
        {
            ExceptionState exceptionState(ExceptionState::GetterContext, "response", "XMLHttpRequest", info.Holder(), info.GetIsolate());
            Document* document = xmlHttpRequest->responseXML(exceptionState);
            if (exceptionState.throwIfNeeded())
                return;
            v8SetReturnValueFast(info, document, xmlHttpRequest);
            return;
        }

    case XMLHttpRequest::ResponseTypeBlob:
        {
            Blob* blob = xmlHttpRequest->responseBlob();
            v8SetReturnValueFast(info, blob, xmlHttpRequest);
            return;
        }

    case XMLHttpRequest::ResponseTypeLegacyStream:
        {
            Stream* stream = xmlHttpRequest->responseLegacyStream();
            v8SetReturnValueFast(info, stream, xmlHttpRequest);
            return;
        }

    case XMLHttpRequest::ResponseTypeArrayBuffer:
        {
            DOMArrayBuffer* arrayBuffer = xmlHttpRequest->responseArrayBuffer();
            v8SetReturnValueFast(info, arrayBuffer, xmlHttpRequest);
            return;
        }
    }
}

} // namespace blink
