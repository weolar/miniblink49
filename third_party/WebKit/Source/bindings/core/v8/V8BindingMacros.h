/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef V8BindingMacros_h
#define V8BindingMacros_h

#if ENABLE_WKE
#include "wke/wkeRecordExceptionInfo.h"
#endif

namespace blink {

// type is an instance of class template V8StringResource<>,
// but Mode argument varies; using type (not Mode) for consistency
// with other macros and ease of code generation
#define TOSTRING_VOID(type, var, value) \
    type var(value);                    \
    if (UNLIKELY(!var.prepare()))       \
        return;

#define TOSTRING_DEFAULT(type, var, value, retVal) \
    type var(value);                               \
    if (UNLIKELY(!var.prepare()))                  \
        return retVal;

template <typename T>
inline bool v8Call(v8::Maybe<T> maybe, T& outVariable)
{
    if (maybe.IsNothing())
        return false;
    outVariable = maybe.FromJust();
    return true;
}

inline bool v8CallBoolean(v8::Maybe<bool> maybe)
{
    bool result;
    return v8Call(maybe, result) && result;
}

template <typename T>
inline bool v8Call(v8::Maybe<T> maybe, T& outVariable, v8::TryCatch& tryCatch)
{
    bool success = v8Call(maybe, outVariable);
    if (tryCatch.HasCaught())
        wke::recordJsExceptionInfo(tryCatch);
    ASSERT(success || tryCatch.HasCaught());
    return success;
}

template <typename T>
inline bool v8Call(v8::MaybeLocal<T> maybeLocal, v8::Local<T>& outVariable, v8::TryCatch& tryCatch)
{
    bool success = maybeLocal.ToLocal(&outVariable);
    if (tryCatch.HasCaught())
        wke::recordJsExceptionInfo(tryCatch);
    ASSERT(success || tryCatch.HasCaught());
    return success;
}

template <typename T>
inline T v8CallOrCrash(v8::Maybe<T> maybe)
{
    return maybe.FromJust();
}

template <typename T>
inline v8::Local<T> v8CallOrCrash(v8::MaybeLocal<T> maybeLocal)
{
    return maybeLocal.ToLocalChecked();
}

// The last "else" is to avoid dangling else problem.
#define V8_CALL(outVariable, handle, methodCall, failureExpression)                \
    if (handle.IsEmpty() || !v8Call(handle->methodCall, outVariable)) { \
        failureExpression;                                                         \
    } else

} // namespace blink

#endif // V8BindingMacros_h
