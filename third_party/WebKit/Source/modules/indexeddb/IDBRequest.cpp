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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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
#include "modules/indexeddb/IDBRequest.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "bindings/modules/v8/V8BindingForModules.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/EventQueue.h"
#include "modules/IndexedDBNames.h"
#include "modules/indexeddb/IDBCursorWithValue.h"
#include "modules/indexeddb/IDBDatabase.h"
#include "modules/indexeddb/IDBEventDispatcher.h"
#include "modules/indexeddb/IDBTracing.h"
#include "modules/indexeddb/IDBValue.h"
#include "platform/SharedBuffer.h"
#include "public/platform/WebBlobInfo.h"

using blink::WebIDBCursor;

namespace blink {

IDBRequest* IDBRequest::create(ScriptState* scriptState, IDBAny* source, IDBTransaction* transaction)
{
    IDBRequest* request = new IDBRequest(scriptState, source, transaction);
    request->suspendIfNeeded();
    // Requests associated with IDBFactory (open/deleteDatabase/getDatabaseNames) are not associated with transactions.
    if (transaction)
        transaction->registerRequest(request);
    return request;
}

IDBRequest::IDBRequest(ScriptState* scriptState, IDBAny* source, IDBTransaction* transaction)
    : ActiveDOMObject(scriptState->executionContext())
    , m_contextStopped(false)
    , m_transaction(transaction)
    , m_readyState(PENDING)
    , m_requestAborted(false)
    , m_scriptState(scriptState)
    , m_source(source)
    , m_hasPendingActivity(true)
    , m_cursorType(IndexedDB::CursorKeyAndValue)
    , m_cursorDirection(WebIDBCursorDirectionNext)
    , m_pendingCursor(nullptr)
    , m_didFireUpgradeNeededEvent(false)
    , m_preventPropagation(false)
    , m_resultDirty(true)
{
}

IDBRequest::~IDBRequest()
{
    ASSERT(m_readyState == DONE || m_readyState == EarlyDeath || !executionContext());
}

DEFINE_TRACE(IDBRequest)
{
    visitor->trace(m_transaction);
    visitor->trace(m_source);
    visitor->trace(m_result);
    visitor->trace(m_error);
#if ENABLE(OILPAN)
    visitor->trace(m_enqueuedEvents);
#endif
    visitor->trace(m_pendingCursor);
    visitor->trace(m_cursorKey);
    visitor->trace(m_cursorPrimaryKey);
    RefCountedGarbageCollectedEventTargetWithInlineData<IDBRequest>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

ScriptValue IDBRequest::result(ExceptionState& exceptionState)
{
    if (m_readyState != DONE) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::requestNotFinishedErrorMessage);
        return ScriptValue();
    }
    if (m_contextStopped || !executionContext())
        return ScriptValue();
    m_resultDirty = false;
    ScriptValue value = ScriptValue::from(m_scriptState.get(), m_result);
    return value;
}

DOMError* IDBRequest::error(ExceptionState& exceptionState) const
{
    if (m_readyState != DONE) {
        exceptionState.throwDOMException(InvalidStateError, IDBDatabase::requestNotFinishedErrorMessage);
        return nullptr;
    }
    return m_error;
}

ScriptValue IDBRequest::source() const
{
    if (m_contextStopped || !executionContext())
        return ScriptValue();

    return ScriptValue::from(m_scriptState.get(), m_source);
}

const String& IDBRequest::readyState() const
{
    ASSERT(m_readyState == PENDING || m_readyState == DONE);

    if (m_readyState == PENDING)
        return IndexedDBNames::pending;

    return IndexedDBNames::done;
}

void IDBRequest::abort()
{
    ASSERT(!m_requestAborted);
    if (m_contextStopped || !executionContext())
        return;
    ASSERT(m_readyState == PENDING || m_readyState == DONE);
    if (m_readyState == DONE)
        return;

    EventQueue* eventQueue = executionContext()->eventQueue();
    for (size_t i = 0; i < m_enqueuedEvents.size(); ++i) {
        bool removed = eventQueue->cancelEvent(m_enqueuedEvents[i].get());
        ASSERT_UNUSED(removed, removed);
    }
    m_enqueuedEvents.clear();

    m_error.clear();
    m_result.clear();
    onError(DOMError::create(AbortError, "The transaction was aborted, so the request cannot be fulfilled."));
    m_requestAborted = true;
}

void IDBRequest::setCursorDetails(IndexedDB::CursorType cursorType, WebIDBCursorDirection direction)
{
    ASSERT(m_readyState == PENDING);
    ASSERT(!m_pendingCursor);
    m_cursorType = cursorType;
    m_cursorDirection = direction;
}

void IDBRequest::setPendingCursor(IDBCursor* cursor)
{
    ASSERT(m_readyState == DONE);
    ASSERT(executionContext());
    ASSERT(m_transaction);
    ASSERT(!m_pendingCursor);
    ASSERT(cursor == getResultCursor());

    m_hasPendingActivity = true;
    m_pendingCursor = cursor;
    setResult(nullptr);
    m_readyState = PENDING;
    m_error.clear();
    m_transaction->registerRequest(this);
}

IDBCursor* IDBRequest::getResultCursor() const
{
    if (!m_result)
        return nullptr;
    if (m_result->type() == IDBAny::IDBCursorType)
        return m_result->idbCursor();
    if (m_result->type() == IDBAny::IDBCursorWithValueType)
        return m_result->idbCursorWithValue();
    return nullptr;
}

void IDBRequest::setResultCursor(IDBCursor* cursor, IDBKey* key, IDBKey* primaryKey, PassRefPtr<IDBValue> value)
{
    ASSERT(m_readyState == PENDING);
    m_cursorKey = key;
    m_cursorPrimaryKey = primaryKey;
    m_cursorValue = value;
    ackReceivedBlobs(m_cursorValue.get());

    onSuccessInternal(IDBAny::create(cursor));
}

void IDBRequest::ackReceivedBlobs(const IDBValue* value)
{
    if (!m_transaction || !m_transaction->backendDB())
        return;
    Vector<String> uuids = value->getUUIDs();
    if (!uuids.isEmpty())
        m_transaction->backendDB()->ackReceivedBlobs(uuids);
}

void IDBRequest::ackReceivedBlobs(const Vector<RefPtr<IDBValue>>& values)
{
    for (size_t i = 0; i < values.size(); ++i)
        ackReceivedBlobs(values[i].get());
}

bool IDBRequest::shouldEnqueueEvent() const
{
    if (m_contextStopped || !executionContext())
        return false;
    ASSERT(m_readyState == PENDING || m_readyState == DONE);
    if (m_requestAborted)
        return false;
    ASSERT(m_readyState == PENDING);
    ASSERT(!m_error && !m_result);
    return true;
}

void IDBRequest::onError(DOMError* error)
{
    IDB_TRACE("IDBRequest::onError()");
    if (!shouldEnqueueEvent())
        return;

    m_error = error;
    setResult(IDBAny::createUndefined());
    m_pendingCursor.clear();
    enqueueEvent(Event::createCancelableBubble(EventTypeNames::error));
}

void IDBRequest::onSuccess(const Vector<String>& stringList)
{
    IDB_TRACE("IDBRequest::onSuccess(StringList)");
    if (!shouldEnqueueEvent())
        return;

    RefPtrWillBeRawPtr<DOMStringList> domStringList = DOMStringList::create(DOMStringList::IndexedDB);
    for (size_t i = 0; i < stringList.size(); ++i)
        domStringList->append(stringList[i]);
    onSuccessInternal(IDBAny::create(domStringList.release()));
}

void IDBRequest::onSuccess(PassOwnPtr<WebIDBCursor> backend, IDBKey* key, IDBKey* primaryKey, PassRefPtr<IDBValue> value)
{
    IDB_TRACE("IDBRequest::onSuccess(IDBCursor)");
    if (!shouldEnqueueEvent())
        return;

    ASSERT(!m_pendingCursor);
    IDBCursor* cursor = nullptr;
    switch (m_cursorType) {
    case IndexedDB::CursorKeyOnly:
        cursor = IDBCursor::create(backend, m_cursorDirection, this, m_source.get(), m_transaction.get());
        break;
    case IndexedDB::CursorKeyAndValue:
        cursor = IDBCursorWithValue::create(backend, m_cursorDirection, this, m_source.get(), m_transaction.get());
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    setResultCursor(cursor, key, primaryKey, value);
}

void IDBRequest::onSuccess(IDBKey* idbKey)
{
    IDB_TRACE("IDBRequest::onSuccess(IDBKey)");
    if (!shouldEnqueueEvent())
        return;

    if (idbKey && idbKey->isValid())
        onSuccessInternal(IDBAny::create(idbKey));
    else
        onSuccessInternal(IDBAny::createUndefined());
}

void IDBRequest::onSuccess(const Vector<RefPtr<IDBValue>>& values)
{
    IDB_TRACE("IDBRequest::onSuccess([IDBValue])");
    if (!shouldEnqueueEvent())
        return;

    ackReceivedBlobs(values);
    onSuccessInternal(IDBAny::create(values));
}

#if ENABLE(ASSERT)
static IDBObjectStore* effectiveObjectStore(IDBAny* source)
{
    if (source->type() == IDBAny::IDBObjectStoreType)
        return source->idbObjectStore();
    if (source->type() == IDBAny::IDBIndexType)
        return source->idbIndex()->objectStore();

    ASSERT_NOT_REACHED();
    return nullptr;
}
#endif

void IDBRequest::onSuccess(PassRefPtr<IDBValue> prpValue)
{
    IDB_TRACE("IDBRequest::onSuccess(IDBValue)");
    if (!shouldEnqueueEvent())
        return;

    RefPtr<IDBValue> value(prpValue);
    ackReceivedBlobs(value.get());

    if (m_pendingCursor) {
        // Value should be null, signifying the end of the cursor's range.
        ASSERT(value->isNull());
        ASSERT(!value->blobInfo()->size());
        m_pendingCursor->close();
        m_pendingCursor.clear();
    }

#if ENABLE(ASSERT)
    if (value->primaryKey()) {
        ASSERT(value->keyPath() == effectiveObjectStore(m_source)->metadata().keyPath);
        assertPrimaryKeyValidOrInjectable(m_scriptState.get(), value.get());
    }
#endif

    onSuccessInternal(IDBAny::create(value.release()));
}

void IDBRequest::onSuccess(int64_t value)
{
    IDB_TRACE("IDBRequest::onSuccess(int64_t)");
    if (!shouldEnqueueEvent())
        return;
    onSuccessInternal(IDBAny::create(value));
}

void IDBRequest::onSuccess()
{
    IDB_TRACE("IDBRequest::onSuccess()");
    if (!shouldEnqueueEvent())
        return;
    onSuccessInternal(IDBAny::createUndefined());
}

void IDBRequest::onSuccessInternal(IDBAny* result)
{
    ASSERT(!m_contextStopped);
    ASSERT(!m_pendingCursor);
    setResult(result);
    enqueueEvent(Event::create(EventTypeNames::success));
}

void IDBRequest::setResult(IDBAny* result)
{
    m_result = result;
    m_resultDirty = true;
}

void IDBRequest::onSuccess(IDBKey* key, IDBKey* primaryKey, PassRefPtr<IDBValue> value)
{
    IDB_TRACE("IDBRequest::onSuccess(key, primaryKey, value)");
    if (!shouldEnqueueEvent())
        return;

    ASSERT(m_pendingCursor);
    setResultCursor(m_pendingCursor.release(), key, primaryKey, value);
}

bool IDBRequest::hasPendingActivity() const
{
    // FIXME: In an ideal world, we should return true as long as anyone has a or can
    //        get a handle to us and we have event listeners. This is order to handle
    //        user generated events properly.
    return m_hasPendingActivity && !m_contextStopped;
}

void IDBRequest::stop()
{
    if (m_contextStopped)
        return;

    m_contextStopped = true;

    if (m_readyState == PENDING) {
        m_readyState = EarlyDeath;
        if (m_transaction) {
            m_transaction->unregisterRequest(this);
            m_transaction.clear();
        }
    }

    m_enqueuedEvents.clear();
    if (m_source)
        m_source->contextWillBeDestroyed();
    if (m_result)
        m_result->contextWillBeDestroyed();
    if (m_pendingCursor)
        m_pendingCursor->contextWillBeDestroyed();
}

const AtomicString& IDBRequest::interfaceName() const
{
    return EventTargetNames::IDBRequest;
}

ExecutionContext* IDBRequest::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

bool IDBRequest::dispatchEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    IDB_TRACE("IDBRequest::dispatchEvent");
    if (m_contextStopped || !executionContext())
        return false;
    ASSERT(m_readyState == PENDING);
    ASSERT(m_hasPendingActivity);
    ASSERT(m_enqueuedEvents.size());
    ASSERT(event->target() == this);

    ScriptState::Scope scope(m_scriptState.get());

    if (event->type() != EventTypeNames::blocked)
        m_readyState = DONE;
    dequeueEvent(event.get());

    WillBeHeapVector<RefPtrWillBeMember<EventTarget>> targets;
    targets.append(this);
    if (m_transaction && !m_preventPropagation) {
        targets.append(m_transaction);
        // If there ever are events that are associated with a database but
        // that do not have a transaction, then this will not work and we need
        // this object to actually hold a reference to the database (to ensure
        // it stays alive).
        targets.append(m_transaction->db());
    }

    // Cursor properties should not be updated until the success event is being dispatched.
    IDBCursor* cursorToNotify = nullptr;
    if (event->type() == EventTypeNames::success) {
        cursorToNotify = getResultCursor();
        if (cursorToNotify)
            cursorToNotify->setValueReady(m_cursorKey.release(), m_cursorPrimaryKey.release(), m_cursorValue.release());
    }

    if (event->type() == EventTypeNames::upgradeneeded) {
        ASSERT(!m_didFireUpgradeNeededEvent);
        m_didFireUpgradeNeededEvent = true;
    }

    // FIXME: When we allow custom event dispatching, this will probably need to change.
    ASSERT_WITH_MESSAGE(event->type() == EventTypeNames::success || event->type() == EventTypeNames::error || event->type() == EventTypeNames::blocked || event->type() == EventTypeNames::upgradeneeded, "event type was %s", event->type().utf8().data());
    const bool setTransactionActive = m_transaction && (event->type() == EventTypeNames::success || event->type() == EventTypeNames::upgradeneeded || (event->type() == EventTypeNames::error && !m_requestAborted));

    if (setTransactionActive)
        m_transaction->setActive(true);

    bool dontPreventDefault = IDBEventDispatcher::dispatch(event.get(), targets);

    if (m_transaction) {
        if (m_readyState == DONE)
            m_transaction->unregisterRequest(this);

        // Possibly abort the transaction. This must occur after unregistering (so this request
        // doesn't receive a second error) and before deactivating (which might trigger commit).
        if (event->type() == EventTypeNames::error && dontPreventDefault && !m_requestAborted) {
            m_transaction->setError(m_error);
            m_transaction->abort(IGNORE_EXCEPTION);
        }

        // If this was the last request in the transaction's list, it may commit here.
        if (setTransactionActive)
            m_transaction->setActive(false);
    }

    if (cursorToNotify)
        cursorToNotify->postSuccessHandlerCallback();

    // An upgradeneeded event will always be followed by a success or error event, so must
    // be kept alive.
    if (m_readyState == DONE && event->type() != EventTypeNames::upgradeneeded)
        m_hasPendingActivity = false;

    return dontPreventDefault;
}

void IDBRequest::uncaughtExceptionInEventHandler()
{
    if (m_transaction && !m_requestAborted) {
        m_transaction->setError(DOMError::create(AbortError, "Uncaught exception in event handler."));
        m_transaction->abort(IGNORE_EXCEPTION);
    }
}

void IDBRequest::transactionDidFinishAndDispatch()
{
    ASSERT(m_transaction);
    ASSERT(m_transaction->isVersionChange());
    ASSERT(m_didFireUpgradeNeededEvent);
    ASSERT(m_readyState == DONE);
    ASSERT(executionContext());
    m_transaction.clear();

    if (m_contextStopped)
        return;

    m_readyState = PENDING;
}

void IDBRequest::enqueueEvent(PassRefPtrWillBeRawPtr<Event> event)
{
    ASSERT(m_readyState == PENDING || m_readyState == DONE);

    if (m_contextStopped || !executionContext())
        return;

    ASSERT_WITH_MESSAGE(m_readyState == PENDING || m_didFireUpgradeNeededEvent, "When queueing event %s, m_readyState was %d", event->type().utf8().data(), m_readyState);

    EventQueue* eventQueue = executionContext()->eventQueue();
    event->setTarget(this);

    // Keep track of enqueued events in case we need to abort prior to dispatch,
    // in which case these must be cancelled. If the events not dispatched for
    // other reasons they must be removed from this list via dequeueEvent().
    if (eventQueue->enqueueEvent(event.get()))
        m_enqueuedEvents.append(event);
}

void IDBRequest::dequeueEvent(Event* event)
{
    for (size_t i = 0; i < m_enqueuedEvents.size(); ++i) {
        if (m_enqueuedEvents[i].get() == event)
            m_enqueuedEvents.remove(i);
    }
}

} // namespace blink
