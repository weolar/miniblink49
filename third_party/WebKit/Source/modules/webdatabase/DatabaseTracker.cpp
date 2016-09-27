/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "modules/webdatabase/DatabaseTracker.h"

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseClient.h"
#include "modules/webdatabase/DatabaseContext.h"
#include "modules/webdatabase/QuotaTracker.h"
#include "modules/webdatabase/sqlite/SQLiteFileSystem.h"
#include "platform/weborigin/DatabaseIdentifier.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "platform/weborigin/SecurityOriginHash.h"
#include "public/platform/Platform.h"
#include "public/platform/WebDatabaseObserver.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/Assertions.h"
#include "wtf/StdLibExtras.h"

namespace blink {

static void databaseClosed(Database* database)
{
    if (Platform::current()->databaseObserver()) {
        Platform::current()->databaseObserver()->databaseClosed(
            createDatabaseIdentifierFromSecurityOrigin(database->securityOrigin()),
            database->stringIdentifier());
    }
}

DatabaseTracker& DatabaseTracker::tracker()
{
    AtomicallyInitializedStaticReference(DatabaseTracker, tracker, new DatabaseTracker);
    return tracker;
}

DatabaseTracker::DatabaseTracker()
{
    SQLiteFileSystem::registerSQLiteVFS();
}

bool DatabaseTracker::canEstablishDatabase(DatabaseContext* databaseContext, const String& name, const String& displayName, unsigned long estimatedSize, DatabaseError& error)
{
    ExecutionContext* executionContext = databaseContext->executionContext();
    bool success = DatabaseClient::from(executionContext)->allowDatabase(executionContext, name, displayName, estimatedSize);
    if (!success)
        error = DatabaseError::GenericSecurityError;
    return success;
}

String DatabaseTracker::fullPathForDatabase(SecurityOrigin* origin, const String& name, bool)
{
    return createDatabaseIdentifierFromSecurityOrigin(origin) + "/" + name + "#";
}

void DatabaseTracker::addOpenDatabase(Database* database)
{
    MutexLocker openDatabaseMapLock(m_openDatabaseMapGuard);
    if (!m_openDatabaseMap)
        m_openDatabaseMap = adoptPtr(new DatabaseOriginMap);

    String originIdentifier = createDatabaseIdentifierFromSecurityOrigin(database->securityOrigin());
    DatabaseNameMap* nameMap = m_openDatabaseMap->get(originIdentifier);
    if (!nameMap) {
        nameMap = new DatabaseNameMap();
        m_openDatabaseMap->set(originIdentifier, nameMap);
    }

    String name(database->stringIdentifier());
    DatabaseSet* databaseSet = nameMap->get(name);
    if (!databaseSet) {
        databaseSet = new DatabaseSet();
        nameMap->set(name, databaseSet);
    }

    databaseSet->add(database);
}

class NotifyDatabaseObserverOnCloseTask final : public ExecutionContextTask {
public:
    static PassOwnPtr<NotifyDatabaseObserverOnCloseTask> create(Database* database)
    {
        return adoptPtr(new NotifyDatabaseObserverOnCloseTask(database));
    }

    void performTask(ExecutionContext*) override
    {
        databaseClosed(m_database.get());
    }

private:
    explicit NotifyDatabaseObserverOnCloseTask(Database* database)
        : m_database(database)
    {
    }

    CrossThreadPersistent<Database> m_database;
};

void DatabaseTracker::removeOpenDatabase(Database* database)
{
    String originIdentifier = createDatabaseIdentifierFromSecurityOrigin(database->securityOrigin());
    MutexLocker openDatabaseMapLock(m_openDatabaseMapGuard);
    ASSERT(m_openDatabaseMap);
    DatabaseNameMap* nameMap = m_openDatabaseMap->get(originIdentifier);
    if (!nameMap)
        return;

    String name(database->stringIdentifier());
    DatabaseSet* databaseSet = nameMap->get(name);
    if (!databaseSet)
        return;

    DatabaseSet::iterator found = databaseSet->find(database);
    if (found == databaseSet->end())
        return;

    databaseSet->remove(found);
    if (databaseSet->isEmpty()) {
        nameMap->remove(name);
        delete databaseSet;
        if (nameMap->isEmpty()) {
            m_openDatabaseMap->remove(originIdentifier);
            delete nameMap;
        }
    }

    ExecutionContext* executionContext = database->databaseContext()->executionContext();
    if (!executionContext->isContextThread())
        executionContext->postTask(FROM_HERE, NotifyDatabaseObserverOnCloseTask::create(database));
    else
        databaseClosed(database);
}

void DatabaseTracker::prepareToOpenDatabase(Database* database)
{
    ASSERT(database->databaseContext()->executionContext()->isContextThread());
    if (Platform::current()->databaseObserver()) {
        Platform::current()->databaseObserver()->databaseOpened(
            createDatabaseIdentifierFromSecurityOrigin(database->securityOrigin()),
            database->stringIdentifier(),
            database->displayName(),
            database->estimatedSize());
    }
}

void DatabaseTracker::failedToOpenDatabase(Database* database)
{
    ExecutionContext* executionContext = database->databaseContext()->executionContext();
    if (!executionContext->isContextThread())
        executionContext->postTask(FROM_HERE, NotifyDatabaseObserverOnCloseTask::create(database));
    else
        databaseClosed(database);
}

unsigned long long DatabaseTracker::getMaxSizeForDatabase(const Database* database)
{
    unsigned long long spaceAvailable = 0;
    unsigned long long databaseSize = 0;
    QuotaTracker::instance().getDatabaseSizeAndSpaceAvailableToOrigin(
        createDatabaseIdentifierFromSecurityOrigin(database->securityOrigin()),
        database->stringIdentifier(), &databaseSize, &spaceAvailable);
    return databaseSize + spaceAvailable;
}

class DatabaseTracker::CloseOneDatabaseImmediatelyTask final : public ExecutionContextTask {
public:
    static PassOwnPtr<CloseOneDatabaseImmediatelyTask> create(const String& originIdentifier, const String& name, Database* database)
    {
        return adoptPtr(new CloseOneDatabaseImmediatelyTask(originIdentifier, name, database));
    }

    void performTask(ExecutionContext*) override
    {
        DatabaseTracker::tracker().closeOneDatabaseImmediately(m_originIdentifier, m_name, m_database);
    }

private:
    CloseOneDatabaseImmediatelyTask(const String& originIdentifier, const String& name, Database* database)
        : m_originIdentifier(originIdentifier.isolatedCopy())
        , m_name(name.isolatedCopy())
        , m_database(database)
    {
    }

    String m_originIdentifier;
    String m_name;
    CrossThreadPersistent<Database> m_database;
};

void DatabaseTracker::closeDatabasesImmediately(const String& originIdentifier, const String& name)
{
    MutexLocker openDatabaseMapLock(m_openDatabaseMapGuard);
    if (!m_openDatabaseMap)
        return;

    DatabaseNameMap* nameMap = m_openDatabaseMap->get(originIdentifier);
    if (!nameMap)
        return;

    DatabaseSet* databaseSet = nameMap->get(name);
    if (!databaseSet)
        return;

    // We have to call closeImmediately() on the context thread.
    for (DatabaseSet::iterator it = databaseSet->begin(); it != databaseSet->end(); ++it)
        (*it)->databaseContext()->executionContext()->postTask(FROM_HERE, CloseOneDatabaseImmediatelyTask::create(originIdentifier, name, *it));
}

void DatabaseTracker::closeOneDatabaseImmediately(const String& originIdentifier, const String& name, Database* database)
{
    // First we have to confirm the 'database' is still in our collection.
    {
        MutexLocker openDatabaseMapLock(m_openDatabaseMapGuard);
        if (!m_openDatabaseMap)
            return;

        DatabaseNameMap* nameMap = m_openDatabaseMap->get(originIdentifier);
        if (!nameMap)
            return;

        DatabaseSet* databaseSet = nameMap->get(name);
        if (!databaseSet)
            return;

        DatabaseSet::iterator found = databaseSet->find(database);
        if (found == databaseSet->end())
            return;
    }

    // And we have to call closeImmediately() without our collection lock being held.
    database->closeImmediately();
}

} // namespace blink
