/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/indexeddb/IDBKeyRange.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "bindings/modules/v8/V8BindingForModules.h"
#include "core/dom/ExceptionCode.h"
#include "modules/indexeddb/IDBDatabase.h"

namespace blink {

IDBKeyRange* IDBKeyRange::fromScriptValue(ExecutionContext* context, const ScriptValue& value, ExceptionState& exceptionState)
{
    if (value.isUndefined() || value.isNull())
        return nullptr;

    IDBKeyRange* range = ScriptValue::to<IDBKeyRange*>(toIsolate(context), value, exceptionState);
    if (range)
        return range;

    IDBKey* key = ScriptValue::to<IDBKey*>(toIsolate(context), value, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!key || !key->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    return new IDBKeyRange(key, key, LowerBoundClosed, UpperBoundClosed);
}

IDBKeyRange::IDBKeyRange(IDBKey* lower, IDBKey* upper, LowerBoundType lowerType, UpperBoundType upperType)
    : m_lower(lower)
    , m_upper(upper)
    , m_lowerType(lowerType)
    , m_upperType(upperType)
{
}

DEFINE_TRACE(IDBKeyRange)
{
    visitor->trace(m_lower);
    visitor->trace(m_upper);
}

ScriptValue IDBKeyRange::lowerValue(ScriptState* scriptState) const
{
    return ScriptValue::from(scriptState, m_lower);
}

ScriptValue IDBKeyRange::upperValue(ScriptState* scriptState) const
{
    return ScriptValue::from(scriptState, m_upper);
}

IDBKeyRange* IDBKeyRange::only(IDBKey* key, ExceptionState& exceptionState)
{
    if (!key || !key->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    return IDBKeyRange::create(key, key, LowerBoundClosed, UpperBoundClosed);
}

IDBKeyRange* IDBKeyRange::only(ExecutionContext* context, const ScriptValue& keyValue, ExceptionState& exceptionState)
{
    IDBKey* key = ScriptValue::to<IDBKey*>(toIsolate(context), keyValue, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!key || !key->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    return IDBKeyRange::create(key, key, LowerBoundClosed, UpperBoundClosed);
}

IDBKeyRange* IDBKeyRange::lowerBound(ExecutionContext* context, const ScriptValue& boundValue, bool open, ExceptionState& exceptionState)
{
    IDBKey* bound = ScriptValue::to<IDBKey*>(toIsolate(context), boundValue, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!bound || !bound->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    return IDBKeyRange::create(bound, nullptr, open ? LowerBoundOpen : LowerBoundClosed, UpperBoundOpen);
}

IDBKeyRange* IDBKeyRange::upperBound(ExecutionContext* context, const ScriptValue& boundValue, bool open, ExceptionState& exceptionState)
{
    IDBKey* bound = ScriptValue::to<IDBKey*>(toIsolate(context), boundValue, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!bound || !bound->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    return IDBKeyRange::create(nullptr, bound, LowerBoundOpen, open ? UpperBoundOpen : UpperBoundClosed);
}

IDBKeyRange* IDBKeyRange::bound(ExecutionContext* context, const ScriptValue& lowerValue, const ScriptValue& upperValue, bool lowerOpen, bool upperOpen, ExceptionState& exceptionState)
{
    IDBKey* lower = ScriptValue::to<IDBKey*>(toIsolate(context), lowerValue, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!lower || !lower->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    IDBKey* upper = ScriptValue::to<IDBKey*>(toIsolate(context), upperValue, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!upper || !upper->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return nullptr;
    }

    if (upper->isLessThan(lower)) {
        exceptionState.throwDOMException(DataError, "The lower key is greater than the upper key.");
        return nullptr;
    }
    if (upper->isEqual(lower) && (lowerOpen || upperOpen)) {
        exceptionState.throwDOMException(DataError, "The lower key and upper key are equal and one of the bounds is open.");
        return nullptr;
    }

    return IDBKeyRange::create(lower, upper, lowerOpen ? LowerBoundOpen : LowerBoundClosed, upperOpen ? UpperBoundOpen : UpperBoundClosed);
}

} // namespace blink
