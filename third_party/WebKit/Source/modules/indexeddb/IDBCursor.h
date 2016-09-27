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

#ifndef IDBCursor_h
#define IDBCursor_h

#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/indexeddb/IDBKey.h"
#include "modules/indexeddb/IDBRequest.h"
#include "modules/indexeddb/IndexedDB.h"
#include "public/platform/modules/indexeddb/WebIDBCursor.h"
#include "public/platform/modules/indexeddb/WebIDBTypes.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class ExceptionState;
class IDBAny;
class IDBTransaction;
class IDBValue;
class ScriptState;

class IDBCursor : public GarbageCollectedFinalized<IDBCursor>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static WebIDBCursorDirection stringToDirection(const String& modeString);

    static IDBCursor* create(PassOwnPtr<WebIDBCursor>, WebIDBCursorDirection, IDBRequest*, IDBAny* source, IDBTransaction*);
    virtual ~IDBCursor();
    DECLARE_TRACE();
    void contextWillBeDestroyed() { m_backend.clear(); }

    // Implement the IDL
    const String& direction() const;
    ScriptValue key(ScriptState*);
    ScriptValue primaryKey(ScriptState*);
    ScriptValue value(ScriptState*);
    ScriptValue source(ScriptState*) const;

    IDBRequest* update(ScriptState*, const ScriptValue&, ExceptionState&);
    void advance(unsigned, ExceptionState&);
    void continueFunction(ScriptState*, const ScriptValue& key, ExceptionState&);
    void continuePrimaryKey(ScriptState*, const ScriptValue& key, const ScriptValue& primaryKey, ExceptionState&);
    IDBRequest* deleteFunction(ScriptState*, ExceptionState&);

    bool isKeyDirty() const { return m_keyDirty; }
    bool isPrimaryKeyDirty() const { return m_primaryKeyDirty; }
    bool isValueDirty() const { return m_valueDirty; }

    void continueFunction(IDBKey*, IDBKey* primaryKey, ExceptionState&);
    void postSuccessHandlerCallback();
    bool isDeleted() const;
    void close();
    void setValueReady(IDBKey*, IDBKey* primaryKey, PassRefPtr<IDBValue>);
    IDBKey* idbPrimaryKey() const { return m_primaryKey; }
    IDBRequest* request() const { return m_request.get(); }
    virtual bool isKeyCursor() const { return true; }
    virtual bool isCursorWithValue() const { return false; }

protected:
    IDBCursor(PassOwnPtr<WebIDBCursor>, WebIDBCursorDirection, IDBRequest*, IDBAny* source, IDBTransaction*);

private:
    IDBObjectStore* effectiveObjectStore() const;

    OwnPtr<WebIDBCursor> m_backend;
    Member<IDBRequest> m_request;
    const WebIDBCursorDirection m_direction;
    Member<IDBAny> m_source;
    Member<IDBTransaction> m_transaction;
    bool m_gotValue;
    bool m_keyDirty;
    bool m_primaryKeyDirty;
    bool m_valueDirty;
    Member<IDBKey> m_key;
    Member<IDBKey> m_primaryKey;
    RefPtr<IDBValue> m_value;
};

} // namespace blink

#endif // IDBCursor_h
