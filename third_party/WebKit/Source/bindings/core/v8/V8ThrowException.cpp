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

#include "config.h"
#include "bindings/core/v8/V8ThrowException.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMException.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

static void domExceptionStackGetter(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Value> value;
    if (info.Data().As<v8::Object>()->Get(isolate->GetCurrentContext(), v8AtomicString(isolate, "stack")).ToLocal(&value))
        v8SetReturnValue(info, value);
}

static void domExceptionStackSetter(v8::Local<v8::Name> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Maybe<bool> unused = info.Data().As<v8::Object>()->Set(info.GetIsolate()->GetCurrentContext(), v8AtomicString(info.GetIsolate(), "stack"), value);
    ALLOW_UNUSED_LOCAL(unused);
}

v8::Local<v8::Value> V8ThrowException::createDOMException(v8::Isolate* isolate, int ec, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Local<v8::Object>& creationContext)
{
    if (ec <= 0 || v8::V8::IsExecutionTerminating())
        return v8Undefined();

    ASSERT(ec == SecurityError || unsanitizedMessage.isEmpty());

    if (ec == V8GeneralError)
        return V8ThrowException::createGeneralError(isolate, sanitizedMessage);
    if (ec == V8TypeError)
        return V8ThrowException::createTypeError(isolate, sanitizedMessage);
    if (ec == V8RangeError)
        return V8ThrowException::createRangeError(isolate, sanitizedMessage);
    if (ec == V8SyntaxError)
        return V8ThrowException::createSyntaxError(isolate, sanitizedMessage);
    if (ec == V8ReferenceError)
        return V8ThrowException::createReferenceError(isolate, sanitizedMessage);

    v8::Local<v8::Object> sanitizedCreationContext = creationContext;

    // FIXME: Is the current context always the right choice?
    Frame* frame = toFrameIfNotDetached(creationContext->CreationContext());
    if (!frame || !BindingSecurity::shouldAllowAccessToFrame(isolate, frame, DoNotReportSecurityError))
        sanitizedCreationContext = isolate->GetCurrentContext()->Global();

    v8::TryCatch tryCatch(isolate);
    DOMException* domException = DOMException::create(ec, sanitizedMessage, unsanitizedMessage);
    v8::Local<v8::Value> exception = toV8(domException, sanitizedCreationContext, isolate);

    if (tryCatch.HasCaught()) {
        ASSERT(exception.IsEmpty());
        return tryCatch.Exception();
    }
    ASSERT(!exception.IsEmpty());

    // Attach an Error object to the DOMException. This is then lazily used to get the stack value.
    v8::Local<v8::Value> error = v8::Exception::Error(v8String(isolate, domException->message()));
    ASSERT(!error.IsEmpty());
    v8::Local<v8::Object> exceptionObject = exception.As<v8::Object>();
    v8::Maybe<bool> result = exceptionObject->SetAccessor(isolate->GetCurrentContext(), v8AtomicString(isolate, "stack"), domExceptionStackGetter, domExceptionStackSetter, v8::MaybeLocal<v8::Value>(error));
    ASSERT_UNUSED(result, result.FromJust());
    V8HiddenValue::setHiddenValue(isolate, exceptionObject, V8HiddenValue::error(isolate), error);

    return exception;
}

v8::Local<v8::Value> V8ThrowException::throwDOMException(int ec, const String& sanitizedMessage, const String& unsanitizedMessage, const v8::Local<v8::Object>& creationContext, v8::Isolate* isolate)
{
    ASSERT(ec == SecurityError || unsanitizedMessage.isEmpty());
    v8::Local<v8::Value> exception = createDOMException(isolate, ec, sanitizedMessage, unsanitizedMessage, creationContext);
    if (exception.IsEmpty())
        return v8Undefined();

    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::createGeneralError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::Error(v8String(isolate, message.isNull() ? "Error" : message));
}

v8::Local<v8::Value> V8ThrowException::throwGeneralError(v8::Isolate* isolate, const String& message)
{
    v8::Local<v8::Value> exception = V8ThrowException::createGeneralError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::createTypeError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::TypeError(v8String(isolate, message.isNull() ? "Type error" : message));
}

v8::Local<v8::Value> V8ThrowException::throwTypeError(v8::Isolate* isolate, const String& message)
{
    v8::Local<v8::Value> exception = V8ThrowException::createTypeError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::createRangeError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::RangeError(v8String(isolate, message.isNull() ? "Range error" : message));
}

v8::Local<v8::Value> V8ThrowException::throwRangeError(v8::Isolate* isolate, const String& message)
{
    v8::Local<v8::Value> exception = V8ThrowException::createRangeError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::createSyntaxError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::SyntaxError(v8String(isolate, message.isNull() ? "Syntax error" : message));
}

v8::Local<v8::Value> V8ThrowException::throwSyntaxError(v8::Isolate* isolate, const String& message)
{
    v8::Local<v8::Value> exception = V8ThrowException::createSyntaxError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::createReferenceError(v8::Isolate* isolate, const String& message)
{
    return v8::Exception::ReferenceError(v8String(isolate, message.isNull() ? "Reference error" : message));
}

v8::Local<v8::Value> V8ThrowException::throwReferenceError(v8::Isolate* isolate, const String& message)
{
    v8::Local<v8::Value> exception = V8ThrowException::createReferenceError(isolate, message);
    return V8ThrowException::throwException(exception, isolate);
}

v8::Local<v8::Value> V8ThrowException::throwException(v8::Local<v8::Value> exception, v8::Isolate* isolate)
{
    if (!v8::V8::IsExecutionTerminating())
        isolate->ThrowException(exception);
    return v8::Undefined(isolate);
}

} // namespace blink
