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
#include "modules/indexeddb/IDBIndex.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "bindings/modules/v8/V8BindingForModules.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/indexeddb/IDBDatabase.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBObjectStore.h"
#include "modules/indexeddb/IDBTracing.h"
#include "modules/indexeddb/IDBTransaction.h"
#include "modules/indexeddb/WebIDBCallbacksImpl.h"
#include "public/platform/modules/indexeddb/WebIDBKeyRange.h"

using blink::WebIDBCallbacks;
using blink::WebIDBCursor;
using blink::WebIDBDatabase;

namespace blink {

IDBIndex::IDBIndex(const IDBIndexMetadata& metadata, IDBObjectStore* objectStore, IDBTransaction* transaction)
    : m_metadata(metadata)
    , m_objectStore(objectStore)
    , m_transaction(transaction)
    , m_deleted(false)
{
    ASSERT(m_objectStore);
    ASSERT(m_transaction);
    ASSERT(m_metadata.id != IDBIndexMetadata::InvalidId);
}

IDBIndex::~IDBIndex()
{
}

DEFINE_TRACE(IDBIndex)
{
    visitor->trace(m_objectStore);
    visitor->trace(m_transaction);
}

ScriptValue IDBIndex::keyPath(ScriptState* scriptState) const
{
    return ScriptValue::from(scriptState, m_metadata.keyPath);
}

IDBRequest* IDBIndex::openCursor(ScriptState* scriptState, const ScriptValue& range, const String& directionString, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::openCursor");
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::indexDeletedErrorMessage);
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
    WebIDBCursorDirection direction = IDBCursor::stringToDirection(directionString);
    IDBKeyRange* keyRange = IDBKeyRange::fromScriptValue(scriptState->executionContext(), range, exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    if (!backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    return openCursor(scriptState, keyRange, direction);
}

IDBRequest* IDBIndex::openCursor(ScriptState* scriptState, IDBKeyRange* keyRange, WebIDBCursorDirection direction)
{
    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    request->setCursorDetails(IndexedDB::CursorKeyAndValue, direction);
    backendDB()->openCursor(m_transaction->id(), m_objectStore->id(), m_metadata.id, keyRange, direction, false, WebIDBTaskTypeNormal, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

IDBRequest* IDBIndex::count(ScriptState* scriptState, const ScriptValue& range, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::count");
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::indexDeletedErrorMessage);
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

    IDBKeyRange* keyRange = IDBKeyRange::fromScriptValue(scriptState->executionContext(), range, exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    if (!backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    backendDB()->count(m_transaction->id(), m_objectStore->id(), m_metadata.id, keyRange, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

IDBRequest* IDBIndex::openKeyCursor(ScriptState* scriptState, const ScriptValue& range, const String& directionString, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::openKeyCursor");
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::indexDeletedErrorMessage);
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
    WebIDBCursorDirection direction = IDBCursor::stringToDirection(directionString);
    IDBKeyRange* keyRange = IDBKeyRange::fromScriptValue(scriptState->executionContext(), range, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    request->setCursorDetails(IndexedDB::CursorKeyOnly, direction);
    backendDB()->openCursor(m_transaction->id(), m_objectStore->id(), m_metadata.id, keyRange, direction, true, WebIDBTaskTypeNormal, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

IDBRequest* IDBIndex::get(ScriptState* scriptState, const ScriptValue& key, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::get");
    return getInternal(scriptState, key, exceptionState, false);
}

IDBRequest* IDBIndex::getAll(ScriptState* scriptState, const ScriptValue& range, ExceptionState& exceptionState)
{
    return getAll(scriptState, range, std::numeric_limits<uint32_t>::max(), exceptionState);
}

IDBRequest* IDBIndex::getAll(ScriptState* scriptState, const ScriptValue& range, unsigned long maxCount, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::getAll");
    return getAllInternal(scriptState, range, maxCount, exceptionState, false);
}

IDBRequest* IDBIndex::getAllKeys(ScriptState* scriptState, const ScriptValue& range, ExceptionState& exceptionState)
{
    return getAllKeys(scriptState, range, std::numeric_limits<uint32_t>::max(), exceptionState);
}

IDBRequest* IDBIndex::getAllKeys(ScriptState* scriptState, const ScriptValue& range, uint32_t maxCount, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::getAllKeys");
    return getAllInternal(scriptState, range, maxCount, exceptionState, true /* keyOnly */);
}

IDBRequest* IDBIndex::getKey(ScriptState* scriptState, const ScriptValue& key, ExceptionState& exceptionState)
{
    IDB_TRACE("IDBIndex::getKey");
    return getInternal(scriptState, key, exceptionState, true);
}

IDBRequest* IDBIndex::getInternal(ScriptState* scriptState, const ScriptValue& key, ExceptionState& exceptionState, bool keyOnly)
{
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::indexDeletedErrorMessage);
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

    IDBKeyRange* keyRange = IDBKeyRange::fromScriptValue(scriptState->executionContext(), key, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!keyRange) {
        exceptionState.throwDOMException(DataError, IDBDatabase::noKeyOrKeyRangeErrorMessage);
        return nullptr;
    }
    if (!backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    backendDB()->get(m_transaction->id(), m_objectStore->id(), m_metadata.id, keyRange, keyOnly, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

IDBRequest* IDBIndex::getAllInternal(ScriptState* scriptState, const ScriptValue& range, unsigned long maxCount, ExceptionState& exceptionState, bool keyOnly)
{
    if (!maxCount) {
        exceptionState.throwTypeError(IDBDatabase::notValidMaxCountErrorMessage);
        return nullptr;
    }
    if (isDeleted()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::indexDeletedErrorMessage);
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

    IDBKeyRange* keyRange = IDBKeyRange::fromScriptValue(scriptState->executionContext(), range, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    if (!backendDB()) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::databaseClosedErrorMessage);
        return nullptr;
    }

    IDBRequest* request = IDBRequest::create(scriptState, IDBAny::create(this), m_transaction.get());
    backendDB()->getAll(m_transaction->id(), m_objectStore->id(), m_metadata.id, keyRange, maxCount, keyOnly, WebIDBCallbacksImpl::create(request).leakPtr());
    return request;
}

WebIDBDatabase* IDBIndex::backendDB() const
{
    return m_transaction->backendDB();
}

bool IDBIndex::isDeleted() const
{
    return m_deleted || m_objectStore->isDeleted();
}

} // namespace blink
