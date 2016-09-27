/*
 * Copyright (c) 2009, 2012 Google Inc. All rights reserved.
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

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/modules/v8/V8SQLError.h"
#include "bindings/modules/v8/V8SQLStatementErrorCallback.h"
#include "bindings/modules/v8/V8SQLTransaction.h"
#include "core/dom/ExecutionContext.h"
#include "wtf/Assertions.h"

namespace blink {

bool V8SQLStatementErrorCallback::handleEvent(SQLTransaction* transaction, SQLError* error)
{
    if (!canInvokeCallback())
        return true;

    v8::Isolate* isolate = m_scriptState->isolate();
    if (!m_scriptState->contextIsValid())
        return true;

    ScriptState::Scope scope(m_scriptState.get());

    v8::Local<v8::Value> transactionHandle = toV8(transaction, m_scriptState->context()->Global(), isolate);
    v8::Local<v8::Value> errorHandle = toV8(error, m_scriptState->context()->Global(), isolate);
    if (transactionHandle.IsEmpty() || errorHandle.IsEmpty()) {
        if (!isScriptControllerTerminating())
            CRASH();
        return true;
    }

    ASSERT(transactionHandle->IsObject());

    v8::Local<v8::Value> argv[] = {
        transactionHandle,
        errorHandle
    };

    v8::TryCatch exceptionCatcher;
    exceptionCatcher.SetVerbose(true);

    v8::Local<v8::Value> result;
    // FIXME: This comment doesn't make much sense given what the code is actually doing.
    //
    // Step 6: If the error callback returns false, then move on to the next
    // statement, if any, or onto the next overall step otherwise. Otherwise,
    // the error callback did not return false, or there was no error callback.
    // Jump to the last step in the overall steps.
    if (!ScriptController::callFunction(executionContext(), m_callback.newLocal(isolate), m_scriptState->context()->Global(), WTF_ARRAY_LENGTH(argv), argv, isolate).ToLocal(&result))
        return true;

    bool value;
    V8_CALL(value, result, BooleanValue(isolate->GetCurrentContext()), return true);
    return value;
}

} // namespace blink
