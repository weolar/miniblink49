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
#include "modules/webdatabase/InspectorDatabaseAgent.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/frame/LocalFrame.h"
#include "core/html/VoidCallback.h"
#include "core/inspector/InspectorState.h"
#include "core/loader/DocumentLoader.h"
#include "core/page/Page.h"
#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseClient.h"
#include "modules/webdatabase/InspectorDatabaseResource.h"
#include "modules/webdatabase/SQLError.h"
#include "modules/webdatabase/SQLResultSet.h"
#include "modules/webdatabase/SQLResultSetRowList.h"
#include "modules/webdatabase/SQLStatementCallback.h"
#include "modules/webdatabase/SQLStatementErrorCallback.h"
#include "modules/webdatabase/SQLTransaction.h"
#include "modules/webdatabase/SQLTransactionCallback.h"
#include "modules/webdatabase/SQLTransactionErrorCallback.h"
#include "modules/webdatabase/sqlite/SQLValue.h"
#include "platform/JSONValues.h"
#include "wtf/Vector.h"

typedef blink::InspectorBackendDispatcher::DatabaseCommandHandler::ExecuteSQLCallback ExecuteSQLCallback;

namespace blink {

namespace DatabaseAgentState {
static const char databaseAgentEnabled[] = "databaseAgentEnabled";
};

namespace {

void reportTransactionFailed(ExecuteSQLCallback* requestCallback, SQLError* error)
{
    RefPtr<TypeBuilder::Database::Error> errorObject = TypeBuilder::Database::Error::create()
        .setMessage(error->message())
        .setCode(error->code());
    requestCallback->sendSuccess(nullptr, nullptr, errorObject.release());
}

class StatementCallback final : public SQLStatementCallback {
public:
    static StatementCallback* create(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
    {
        return new StatementCallback(requestCallback);
    }

    ~StatementCallback() override { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_requestCallback);
        SQLStatementCallback::trace(visitor);
    }

    bool handleEvent(SQLTransaction*, SQLResultSet* resultSet) override
    {
        SQLResultSetRowList* rowList = resultSet->rows();

        RefPtr<TypeBuilder::Array<String>> columnNames = TypeBuilder::Array<String>::create();
        const Vector<String>& columns = rowList->columnNames();
        for (size_t i = 0; i < columns.size(); ++i)
            columnNames->addItem(columns[i]);

        RefPtr<TypeBuilder::Array<JSONValue>> values = TypeBuilder::Array<JSONValue>::create();
        const Vector<SQLValue>& data = rowList->values();
        for (size_t i = 0; i < data.size(); ++i) {
            const SQLValue& value = rowList->values()[i];
            switch (value.type()) {
            case SQLValue::StringValue: values->addItem(JSONString::create(value.string())); break;
            case SQLValue::NumberValue: values->addItem(JSONBasicValue::create(value.number())); break;
            case SQLValue::NullValue: values->addItem(JSONValue::null()); break;
            }
        }
        m_requestCallback->sendSuccess(columnNames.release(), values.release(), nullptr);
        return true;
    }

private:
    StatementCallback(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
        : m_requestCallback(requestCallback) { }
    RefPtrWillBeMember<ExecuteSQLCallback> m_requestCallback;
};

class StatementErrorCallback final : public SQLStatementErrorCallback {
public:
    static StatementErrorCallback* create(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
    {
        return new StatementErrorCallback(requestCallback);
    }

    ~StatementErrorCallback() override { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_requestCallback);
        SQLStatementErrorCallback::trace(visitor);
    }

    bool handleEvent(SQLTransaction*, SQLError* error) override
    {
        reportTransactionFailed(m_requestCallback.get(), error);
        return true;
    }

private:
    StatementErrorCallback(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
        : m_requestCallback(requestCallback) { }
    RefPtrWillBeMember<ExecuteSQLCallback> m_requestCallback;
};

class TransactionCallback final : public SQLTransactionCallback {
public:
    static TransactionCallback* create(const String& sqlStatement, PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
    {
        return new TransactionCallback(sqlStatement, requestCallback);
    }

    ~TransactionCallback() override { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_requestCallback);
        SQLTransactionCallback::trace(visitor);
    }

    bool handleEvent(SQLTransaction* transaction) override
    {
        if (!m_requestCallback->isActive())
            return true;

        Vector<SQLValue> sqlValues;
        SQLStatementCallback* callback = StatementCallback::create(m_requestCallback.get());
        SQLStatementErrorCallback* errorCallback = StatementErrorCallback::create(m_requestCallback.get());
        transaction->executeSQL(m_sqlStatement, sqlValues, callback, errorCallback, IGNORE_EXCEPTION);
        return true;
    }
private:
    TransactionCallback(const String& sqlStatement, PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
        : m_sqlStatement(sqlStatement)
        , m_requestCallback(requestCallback) { }
    String m_sqlStatement;
    RefPtrWillBeMember<ExecuteSQLCallback> m_requestCallback;
};

class TransactionErrorCallback final : public SQLTransactionErrorCallback {
public:
    static TransactionErrorCallback* create(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
    {
        return new TransactionErrorCallback(requestCallback);
    }

    ~TransactionErrorCallback() override { }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_requestCallback);
        SQLTransactionErrorCallback::trace(visitor);
    }

    bool handleEvent(SQLError* error) override
    {
        reportTransactionFailed(m_requestCallback.get(), error);
        return true;
    }
private:
    TransactionErrorCallback(PassRefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback)
        : m_requestCallback(requestCallback) { }
    RefPtrWillBeMember<ExecuteSQLCallback> m_requestCallback;
};

class TransactionSuccessCallback final : public VoidCallback {
public:
    static TransactionSuccessCallback* create()
    {
        return new TransactionSuccessCallback();
    }

    ~TransactionSuccessCallback() override { }

    void handleEvent() override { }

private:
    TransactionSuccessCallback() { }
};

} // namespace

void InspectorDatabaseAgent::didOpenDatabase(Database* database, const String& domain, const String& name, const String& version)
{
    if (InspectorDatabaseResource* resource = findByFileName(database->fileName())) {
        resource->setDatabase(database);
        return;
    }

    InspectorDatabaseResource* resource = InspectorDatabaseResource::create(database, domain, name, version);
    m_resources.set(resource->id(), resource);
    // Resources are only bound while visible.
    if (frontend() && m_enabled)
        resource->bind(frontend());
}

void InspectorDatabaseAgent::didCommitLoadForLocalFrame(LocalFrame* frame)
{
    // FIXME(dgozman): adapt this for out-of-process iframes.
    if (frame != m_page->mainFrame())
        return;

    m_resources.clear();
}

InspectorDatabaseAgent::InspectorDatabaseAgent(Page* page)
    : InspectorBaseAgent<InspectorDatabaseAgent, InspectorFrontend::Database>("Database")
    , m_page(page)
    , m_enabled(false)
{
    DatabaseClient::fromPage(page)->setInspectorAgent(this);
}

InspectorDatabaseAgent::~InspectorDatabaseAgent()
{
}

void InspectorDatabaseAgent::enable(ErrorString*)
{
    if (m_enabled)
        return;
    m_enabled = true;
    m_state->setBoolean(DatabaseAgentState::databaseAgentEnabled, m_enabled);

    DatabaseResourcesHeapMap::iterator databasesEnd = m_resources.end();
    for (DatabaseResourcesHeapMap::iterator it = m_resources.begin(); it != databasesEnd; ++it)
        it->value->bind(frontend());
}

void InspectorDatabaseAgent::disable(ErrorString*)
{
    if (!m_enabled)
        return;
    m_enabled = false;
    m_state->setBoolean(DatabaseAgentState::databaseAgentEnabled, m_enabled);
}

void InspectorDatabaseAgent::restore()
{
    m_enabled = m_state->getBoolean(DatabaseAgentState::databaseAgentEnabled);
}

void InspectorDatabaseAgent::getDatabaseTableNames(ErrorString* error, const String& databaseId, RefPtr<TypeBuilder::Array<String>>& names)
{
    if (!m_enabled) {
        *error = "Database agent is not enabled";
        return;
    }

    names = TypeBuilder::Array<String>::create();

    Database* database = databaseForId(databaseId);
    if (database) {
        Vector<String> tableNames = database->tableNames();
        unsigned length = tableNames.size();
        for (unsigned i = 0; i < length; ++i)
            names->addItem(tableNames[i]);
    }
}

void InspectorDatabaseAgent::executeSQL(ErrorString*, const String& databaseId, const String& query, PassRefPtrWillBeRawPtr<ExecuteSQLCallback> prpRequestCallback)
{
    RefPtrWillBeRawPtr<ExecuteSQLCallback> requestCallback = prpRequestCallback;

    if (!m_enabled) {
        requestCallback->sendFailure("Database agent is not enabled");
        return;
    }

    Database* database = databaseForId(databaseId);
    if (!database) {
        requestCallback->sendFailure("Database not found");
        return;
    }

    SQLTransactionCallback* callback = TransactionCallback::create(query, requestCallback.get());
    SQLTransactionErrorCallback* errorCallback = TransactionErrorCallback::create(requestCallback.get());
    VoidCallback* successCallback = TransactionSuccessCallback::create();
    database->transaction(callback, errorCallback, successCallback);
}

InspectorDatabaseResource* InspectorDatabaseAgent::findByFileName(const String& fileName)
{
    for (DatabaseResourcesHeapMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it) {
        if (it->value->database()->fileName() == fileName)
            return it->value.get();
    }
    return 0;
}

Database* InspectorDatabaseAgent::databaseForId(const String& databaseId)
{
    DatabaseResourcesHeapMap::iterator it = m_resources.find(databaseId);
    if (it == m_resources.end())
        return 0;
    return it->value->database();
}

DEFINE_TRACE(InspectorDatabaseAgent)
{
#if ENABLE(OILPAN)
    visitor->trace(m_resources);
#endif
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
