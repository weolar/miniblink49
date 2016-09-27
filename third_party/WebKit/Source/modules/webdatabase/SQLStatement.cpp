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
#include "config.h"
#include "modules/webdatabase/SQLStatement.h"

#include "core/inspector/InspectorInstrumentation.h"
#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseManager.h"
#include "modules/webdatabase/SQLError.h"
#include "modules/webdatabase/SQLStatementBackend.h"
#include "modules/webdatabase/SQLStatementCallback.h"
#include "modules/webdatabase/SQLStatementErrorCallback.h"
#include "modules/webdatabase/SQLTransaction.h"
#include "modules/webdatabase/sqlite/SQLiteDatabase.h"
#include "modules/webdatabase/sqlite/SQLiteStatement.h"
#include "platform/Logging.h"
#include "wtf/text/CString.h"

namespace blink {

SQLStatement* SQLStatement::create(Database* database,
    SQLStatementCallback* callback, SQLStatementErrorCallback* errorCallback)
{
    return new SQLStatement(database, callback, errorCallback);
}

SQLStatement::SQLStatement(Database* database, SQLStatementCallback* callback,
    SQLStatementErrorCallback* errorCallback)
    : m_statementCallback(callback)
    , m_statementErrorCallback(errorCallback)
    , m_asyncOperationId(0)
{
    if (hasCallback() || hasErrorCallback())
        m_asyncOperationId = InspectorInstrumentation::traceAsyncOperationStarting(database->executionContext(), "SQLStatement");
}

SQLStatement::~SQLStatement()
{
}

DEFINE_TRACE(SQLStatement)
{
    visitor->trace(m_backend);
    visitor->trace(m_statementCallback);
    visitor->trace(m_statementErrorCallback);
}

void SQLStatement::setBackend(SQLStatementBackend* backend)
{
    m_backend = backend;
}

bool SQLStatement::hasCallback()
{
    return m_statementCallback;
}

bool SQLStatement::hasErrorCallback()
{
    return m_statementErrorCallback;
}

bool SQLStatement::performCallback(SQLTransaction* transaction)
{
    ASSERT(transaction);
    ASSERT(m_backend);

    bool callbackError = false;

    SQLStatementCallback* callback = m_statementCallback.release();
    SQLStatementErrorCallback* errorCallback = m_statementErrorCallback.release();
    SQLErrorData* error = m_backend->sqlError();

    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncOperationCompletedCallbackStarting(transaction->database()->executionContext(), m_asyncOperationId);

    // Call the appropriate statement callback and track if it resulted in an error,
    // because then we need to jump to the transaction error callback.
    if (error) {
        if (errorCallback)
            callbackError = errorCallback->handleEvent(transaction, SQLError::create(*error));
    } else if (callback) {
        callbackError = !callback->handleEvent(transaction, m_backend->sqlResultSet());
    }

    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);

    return callbackError;
}

} // namespace blink
