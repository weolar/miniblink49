/*
 * Copyright (C) 2007, 2013 Apple Inc. All rights reserved.
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

#ifndef SQLTransaction_h
#define SQLTransaction_h

#include "bindings/core/v8/Nullable.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/V8BindingForModules.h"
#include "modules/webdatabase/SQLStatement.h"
#include "modules/webdatabase/SQLTransactionStateMachine.h"
#include "platform/heap/Handle.h"

namespace blink {

class Database;
class ExceptionState;
class SQLErrorData;
class SQLStatementCallback;
class SQLStatementErrorCallback;
class SQLTransactionBackend;
class SQLTransactionCallback;
class SQLTransactionErrorCallback;
class SQLValue;
class ScriptValue;
class VoidCallback;

class SQLTransaction final
    : public GarbageCollectedFinalized<SQLTransaction>
    , public SQLTransactionStateMachine<SQLTransaction>
    , public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static SQLTransaction* create(Database*, SQLTransactionCallback*,
        VoidCallback* successCallback, SQLTransactionErrorCallback*, bool readOnly);
    ~SQLTransaction();
    DECLARE_TRACE();

    void performPendingCallback();

    void executeSQL(const String& sqlStatement, const Vector<SQLValue>& arguments,
        SQLStatementCallback*, SQLStatementErrorCallback*, ExceptionState&);
    void executeSql(ScriptState*, const String& sqlStatement, ExceptionState&);
    void executeSql(ScriptState*, const String& sqlStatement, const Nullable<Vector<ScriptValue>>& arguments,
        SQLStatementCallback*, SQLStatementErrorCallback*, ExceptionState&);

    Database* database() { return m_database.get(); }

    SQLTransactionErrorCallback* releaseErrorCallback();

    // APIs called from the backend published:
    void requestTransitToState(SQLTransactionState);
    bool hasCallback() const;
    bool hasSuccessCallback() const;
    bool hasErrorCallback() const;
    void setBackend(SQLTransactionBackend*);

private:
    SQLTransaction(Database*, SQLTransactionCallback*,
        VoidCallback* successCallback, SQLTransactionErrorCallback*,
        bool readOnly);

    void clearCallbacks();

    // State Machine functions:
    StateFunction stateFunctionFor(SQLTransactionState) override;
    bool computeNextStateAndCleanupIfNeeded();

    // State functions:
    SQLTransactionState deliverTransactionCallback();
    SQLTransactionState deliverTransactionErrorCallback();
    SQLTransactionState deliverStatementCallback();
    SQLTransactionState deliverQuotaIncreaseCallback();
    SQLTransactionState deliverSuccessCallback();

    SQLTransactionState unreachableState();
    SQLTransactionState sendToBackendState();

    SQLTransactionState nextStateForTransactionError();

    Member<Database> m_database;
    Member<SQLTransactionBackend> m_backend;
    Member<SQLTransactionCallback> m_callback;
    Member<VoidCallback> m_successCallback;
    Member<SQLTransactionErrorCallback> m_errorCallback;

    bool m_executeSqlAllowed;
    OwnPtr<SQLErrorData> m_transactionError;

    bool m_readOnly;
    int m_asyncOperationId;
};

} // namespace blink

#endif // SQLTransaction_h
