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
#include "modules/indexeddb/IDBCursor.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "bindings/modules/v8/V8BindingForModules.h"
#include "core/dom/ExceptionCode.h"
#include "core/inspector/ScriptCallStack.h"
#include "modules/IndexedDBNames.h"
#include "modules/indexeddb/IDBAny.h"
#include "modules/indexeddb/IDBDatabase.h"
#include "modules/indexeddb/IDBObjectStore.h"
#include "modules/indexeddb/IDBTracing.h"
#include "modules/indexeddb/IDBTransaction.h"
#include "modules/indexeddb/WebIDBCallbacksImpl.h"
#include "public/platform/modules/indexeddb/WebIDBDatabase.h"
#include "public/platform/modules/indexeddb/WebIDBKeyRange.h"
#include <limits>

using blink::WebIDBCursor;
using blink::WebIDBDatabase;

namespace blink {

IDBCursor* IDBCursor::create(PassOwnPtr<WebIDBCursor> backend, WebIDBCursorDirection direction, IDBRequest* request, IDBAny* source, IDBTransaction* transaction)
{
    return new IDBCursor(backend, direction, request, source, transaction);
}

IDBCursor::IDBCursor(PassOwnPtr<WebIDBCursor> backend, WebIDBCursorDirection direction, IDBRequest* request, IDBAny* source, IDBTransaction* transaction)
    : m_backend(backend)
    , m_request(request)
    , m_direction(direction)
    , m_source(source)
    , m_transaction(transaction)
    , m_gotValue(false)
    , m_keyDirty(true)
    , m_primaryKeyDirty(true)
    , m_valueDirty(true)
{
    ASSERT(m_backend);
    ASSERT(m_request);
    ASSERT(m_source->type() == IDBAny::IDBObjectStoreType || m_source->type() == IDBAny::IDBIndexType);
    ASSERT(m_transaction);
}

IDBCursor::~IDBCursor()
{
}

DEFINE_TRACE(IDBCursor)
{
    visitor->trace(m_request);
    visitor->trace(m_source);
    visitor->trace(m_transaction);
    visitor->trace(m_key);
    visitor->trace(m_primaryKey);
}

IDBRequest* IDBCursor::update(ScriptState* scriptState, const ScriptValue& value, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBCursor::update");

    if (!m_gotValue) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::noValueErrorMessage);
        return nullptr;
    }
    if (isKeyCursor()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::isKeyCursorErrorMessage);
        return nullptr;
    }
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::sourceDeletedErrorMessage);
        return nullptr;
    }
    if (m_transaction->isFinished() || m_transaction->isFinishing()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionFinishedErrorMessage);
        return nullptr;
    }
    if (!m_transaction->isActive()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionInactiveErrorMessage);
        return nullptr;
    }
    if (m_transaction->isReadOnly()) {
        exceptionState.throwDOMException(ReadOnlyError, "The record may not be updated inside a read-only transaction.");
        return nullptr;
    }

    IDBObjectStore* objectStore = effectiveObjectStore();
    return objectStore->put(scriptState, WebIDBPutModeCursorUpdate, IDBAny::create(this), value, m_primaryKey, exceptionState);
}

void IDBCursor::advance(unsigned count, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBCursor::advance");
    if (!count) {
        exceptionState.throwTypeError("A count argument with value 0 (zero) was supplied, must be greater than 0.");
        return;
    }
    if (!m_gotValue) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::noValueErrorMessage);
        return;
    }
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::sourceDeletedErrorMessage);
        return;
    }

    if (m_transaction->isFinished() || m_transaction->isFinishing()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionFinishedErrorMessage);
        return;
    }
    if (!m_transaction->isActive()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionInactiveErrorMessage);
        return;
    }

    m_request->setPendingCursor(this);
    m_gotValue = false;
    m_backend->advance(count, WebIDBCallbacksImpl::create(m_request).leakPtr());
}

void IDBCursor::continueFunction(ScriptState* scriptState, const ScriptValue& keyValue, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBCursor::continue");
    IDBKey* key = keyValue.isUndefined() || keyValue.isNull() ? nullptr : ScriptValue::to<IDBKey*>(scriptState->isolate(), keyValue, exceptionState);
    if (exceptionState.hadException())
        return;
    if (key && !key->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return;
    }
    continueFunction(key, nullptr, exceptionState);
}

void IDBCursor::continuePrimaryKey(ScriptState* scriptState, const ScriptValue& keyValue, const ScriptValue& primaryKeyValue, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBCursor::continuePrimaryKey");
    if (m_source->type() != IDBAny::IDBIndexType) {
        exceptionState.throwDOMException(InvalidAccessError, "The cursor's source is not an index.");
        return;
    }
    if (m_direction != WebIDBCursorDirectionNext && m_direction != WebIDBCursorDirectionPrev) {
        exceptionState.throwDOMException(InvalidAccessError, "The cursor's direction is not 'next' or 'prev'.");
        return;
    }

    IDBKey* key = ScriptValue::to<IDBKey*>(scriptState->isolate(), keyValue, exceptionState);
    if (exceptionState.hadException())
        return;
    if (!key->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return;
    }

    IDBKey* primaryKey = ScriptValue::to<IDBKey*>(scriptState->isolate(), primaryKeyValue, exceptionState);
    if (exceptionState.hadException())
        return;
    if (!primaryKey->isValid()) {
        exceptionState.throwDOMException(DataError, IDBDatabase::notValidKeyErrorMessage);
        return;
    }

    continueFunction(key, primaryKey, exceptionState);
}

void IDBCursor::continueFunction(IDBKey* key, IDBKey* primaryKey, ExceptionState& exceptionState)
{
    ASSERT(!primaryKey || (key && primaryKey));

    if (m_transaction->isFinished() || m_transaction->isFinishing()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionFinishedErrorMessage);
        return;
    }
    if (!m_transaction->isActive()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionInactiveErrorMessage);
        return;
    }

    if (!m_gotValue) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::noValueErrorMessage);
        return;
    }

    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::sourceDeletedErrorMessage);
        return;
    }

    if (key) {
        ASSERT(m_key);
        if (m_direction == WebIDBCursorDirectionNext || m_direction == WebIDBCursorDirectionNextNoDuplicate) {
            const bool ok = m_key->isLessThan(key)
                || (primaryKey && m_key->isEqual(key) && m_primaryKey->isLessThan(primaryKey));
            if (!ok) {
                exceptionState.throwDOMException(DataError, "The parameter is less than or equal to this cursor's position.");
                return;
            }

        } else {
            const bool ok = key->isLessThan(m_key.get())
                || (primaryKey && key->isEqual(m_key.get()) && primaryKey->isLessThan(m_primaryKey.get()));
            if (!ok) {
                exceptionState.throwDOMException(DataError, "The parameter is greater than or equal to this cursor's position.");
                return;
            }
        }
    }

    // FIXME: We're not using the context from when continue was called, which means the callback
    //        will be on the original context openCursor was called on. Is this right?
    m_request->setPendingCursor(this);
    m_gotValue = false;
    m_backend->continueFunction(key, primaryKey, WebIDBCallbacksImpl::create(m_request).leakPtr());
}

IDBRequest* IDBCursor::deleteFunction(ScriptState* scriptState, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBCursor::delete");
    if (m_transaction->isFinished() || m_transaction->isFinishing()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionFinishedErrorMessage);
        return nullptr;
    }
    if (!m_transaction->isActive()) {
        exceptionState.throwDOMException(TransactionInactiveError, IDBDatabase::transactionInactiveErrorMessage);
        return nullptr;
    }
    if (m_transaction->isReadOnly()) {
        exceptionState.throwDOMException(ReadOnlyError, "The record may not be deleted inside a read-only transaction.");
        return nullptr;
    }

    if (!m_gotValue) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::noValueErrorMessage);
        return nullptr;
    }
    if (isKeyCursor()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::isKeyCursorErrorMessage);
        return nullptr;
    }
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::sourceDeletedErrorMessage);
        return nullptr;
    }
    if (!m_transaction->backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    IDBKeyRange* keyRange = IDBKeyRange::only(m_primaryKey, exceptionState);
    ASSERT(!exceptionState.hadException());

    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    m_transaction->backendDB()->deleteRange(m_transaction->id(), effectiveObjectStore()->id(), keyRange, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

void IDBCursor::postSuccessHandlerCallback()
{
    if (m_backend)
        m_backend->postSuccessHandlerCallback();
}

void IDBCursor::close()
{
    m_value.clear();
    m_request.clear();
    m_backend.clear();
}

ScriptValue IDBCursor::key(ScriptState* scriptState)
{
    m_keyDirty = false;
    return ScriptValue::from(scriptState, m_key);
}

ScriptValue IDBCursor::primaryKey(ScriptState* scriptState)
{
    m_primaryKeyDirty = false;
    return ScriptValue::from(scriptState, m_primaryKey);
}

ScriptValue IDBCursor::value(ScriptState* scriptState)
{
    ASSERT(isCursorWithValue());

    IDBObjectStore* objectStore = effectiveObjectStore();
    const IDBObjectStoreMetadata& metadata = objectStore->metadata();
    IDBAny* value;
    if (metadata.autoIncrement && !metadata.keyPath.isNull()) {
        RefPtr<IDBValue> idbValue = IDBValue::create(m_value.get(), m_primaryKey, metadata.keyPath);
#if ENABLE(ASSERT)
        assertPrimaryKeyValidOrInjectable(scriptState, idbValue.get());
#endif
        value = IDBAny::create(idbValue.release());
    } else {
        value = IDBAny::create(m_value);
    }

    m_valueDirty = false;
    ScriptValue scriptValue = ScriptValue::from(scriptState, value);
    return scriptValue;
}

ScriptValue IDBCursor::source(ScriptState* scriptState) const
{
    return ScriptValue::from(scriptState, m_source);
}

void IDBCursor::setValueReady(IDBKey* key, IDBKey* primaryKey, PassRefPtr<IDBValue> value)
{
    m_key = key;
    m_keyDirty = true;

    m_primaryKey = primaryKey;
    m_primaryKeyDirty = true;

    if (isCursorWithValue()) {
        m_value = value;
        m_valueDirty = true;
    }

    m_gotValue = true;
}

IDBObjectStore* IDBCursor::effectiveObjectStore() const
{
    if (m_source->type() == IDBAny::IDBObjectStoreType)
        return m_source->idbObjectStore();
    return m_source->idbIndex()->objectStore();
}

bool IDBCursor::isDeleted() const
{
    if (m_source->type() == IDBAny::IDBObjectStoreType)
        return m_source->idbObjectStore()->isDeleted();
    return m_source->idbIndex()->isDeleted();
}

WebIDBCursorDirection IDBCursor::stringToDirection(const String& directionString)
{
    if (directionString == IndexedDBNames::next)
        return WebIDBCursorDirectionNext;
    if (directionString == IndexedDBNames::nextunique)
        return WebIDBCursorDirectionNextNoDuplicate;
    if (directionString == IndexedDBNames::prev)
        return WebIDBCursorDirectionPrev;
    if (directionString == IndexedDBNames::prevunique)
        return WebIDBCursorDirectionPrevNoDuplicate;

    ASSERT_NOT_REACHED();
    return WebIDBCursorDirectionNext;
}

const String& IDBCursor::direction() const
{
    switch (m_direction) {
    case WebIDBCursorDirectionNext:
        return IndexedDBNames::next;

    case WebIDBCursorDirectionNextNoDuplicate:
        return IndexedDBNames::nextunique;

    case WebIDBCursorDirectionPrev:
        return IndexedDBNames::prev;

    case WebIDBCursorDirectionPrevNoDuplicate:
        return IndexedDBNames::prevunique;

    default:
        ASSERT_NOT_REACHED();
        return IndexedDBNames::next;
    }
}

} // namespace blink
