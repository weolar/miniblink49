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

#ifndef IDBRequest_h
#define IDBRequest_h

#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/dom/DOMError.h"
#include "core/dom/DOMStringList.h"
#include "core/events/EventListener.h"
#include "core/events/EventTarget.h"
#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/indexeddb/IDBAny.h"
#include "modules/indexeddb/IDBTransaction.h"
#include "modules/indexeddb/IndexedDB.h"
#include "platform/blob/BlobData.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebBlobInfo.h"
#include "public/platform/modules/indexeddb/WebIDBCursor.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"

namespace blink {

class ExceptionState;
class IDBCursor;
struct IDBDatabaseMetadata;
class IDBValue;

class MODULES_EXPORT IDBRequest
    : public RefCountedGarbageCollectedEventTargetWithInlineData<IDBRequest>
    , public ActiveDOMObject {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(IDBRequest);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(IDBRequest);
public:
    static IDBRequest* create(ScriptState*, IDBAny* source, IDBTransaction*);
    ~IDBRequest() override;
    DECLARE_VIRTUAL_TRACE();

    ScriptState* scriptState() { return m_scriptState.get(); }
    ScriptValue result(ExceptionState&);
    DOMError* error(ExceptionState&) const;
    ScriptValue source() const;
    IDBTransaction* transaction() const { return m_transaction.get(); }

    bool isResultDirty() const { return m_resultDirty; }
    IDBAny* resultAsAny() const { return m_result; }

    // Requests made during index population are implementation details and so
    // events should not be visible to script.
    void preventPropagation() { m_preventPropagation = true; }

    // Defined in the IDL
    enum ReadyState {
        PENDING = 1,
        DONE = 2,
        EarlyDeath = 3
    };

    const String& readyState() const;

    DEFINE_ATTRIBUTE_EVENT_LISTENER(success);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);

    void setCursorDetails(IndexedDB::CursorType, WebIDBCursorDirection);
    void setPendingCursor(IDBCursor*);
    void abort();

    virtual void onError(DOMError*);
    virtual void onSuccess(const Vector<String>&);
    virtual void onSuccess(PassOwnPtr<WebIDBCursor>, IDBKey*, IDBKey* primaryKey, PassRefPtr<IDBValue>);
    virtual void onSuccess(IDBKey*);
    virtual void onSuccess(PassRefPtr<IDBValue>);
    virtual void onSuccess(const Vector<RefPtr<IDBValue>>&);
    virtual void onSuccess(int64_t);
    virtual void onSuccess();
    virtual void onSuccess(IDBKey*, IDBKey* primaryKey, PassRefPtr<IDBValue>);

    // Only IDBOpenDBRequest instances should receive these:
    virtual void onBlocked(int64_t oldVersion) { ASSERT_NOT_REACHED(); }
    virtual void onUpgradeNeeded(int64_t oldVersion, PassOwnPtr<WebIDBDatabase>, const IDBDatabaseMetadata&, WebIDBDataLoss, String dataLossMessage) { ASSERT_NOT_REACHED(); }
    virtual void onSuccess(PassOwnPtr<WebIDBDatabase>, const IDBDatabaseMetadata&) { ASSERT_NOT_REACHED(); }

    // ActiveDOMObject
    bool hasPendingActivity() const final;
    void stop() final;

    // EventTarget
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const final;
    void uncaughtExceptionInEventHandler() final;

    using EventTarget::dispatchEvent;
    bool dispatchEvent(PassRefPtrWillBeRawPtr<Event>) override;

    // Called by a version change transaction that has finished to set this
    // request back from DONE (following "upgradeneeded") back to PENDING (for
    // the upcoming "success" or "error").
    void transactionDidFinishAndDispatch();

    IDBCursor* getResultCursor() const;

protected:
    IDBRequest(ScriptState*, IDBAny* source, IDBTransaction*);
    void enqueueEvent(PassRefPtrWillBeRawPtr<Event>);
    void dequeueEvent(Event*);
    virtual bool shouldEnqueueEvent() const;
    void onSuccessInternal(IDBAny*);
    void setResult(IDBAny*);

    bool m_contextStopped;
    Member<IDBTransaction> m_transaction;
    ReadyState m_readyState;
    bool m_requestAborted; // May be aborted by transaction then receive async onsuccess; ignore vs. assert.

private:
    void setResultCursor(IDBCursor*, IDBKey*, IDBKey* primaryKey, PassRefPtr<IDBValue>);
    void ackReceivedBlobs(const IDBValue*);
    void ackReceivedBlobs(const Vector<RefPtr<IDBValue>>&);

    RefPtr<ScriptState> m_scriptState;
    Member<IDBAny> m_source;
    Member<IDBAny> m_result;
    Member<DOMError> m_error;

    bool m_hasPendingActivity;
    WillBeHeapVector<RefPtrWillBeMember<Event>> m_enqueuedEvents;

    // Only used if the result type will be a cursor.
    IndexedDB::CursorType m_cursorType;
    WebIDBCursorDirection m_cursorDirection;
    // When a cursor is continued/advanced, m_result is cleared and m_pendingCursor holds it.
    Member<IDBCursor> m_pendingCursor;
    // New state is not applied to the cursor object until the event is dispatched.
    Member<IDBKey> m_cursorKey;
    Member<IDBKey> m_cursorPrimaryKey;
    RefPtr<IDBValue> m_cursorValue;

    bool m_didFireUpgradeNeededEvent;
    bool m_preventPropagation;
    bool m_resultDirty;
};

} // namespace blink

#endif // IDBRequest_h
