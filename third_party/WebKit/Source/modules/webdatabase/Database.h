/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Database_h
#define Database_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "modules/webdatabase/DatabaseBasicTypes.h"
#include "modules/webdatabase/DatabaseError.h"
#include "modules/webdatabase/sqlite/SQLiteDatabase.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/Deque.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ChangeVersionData;
class DatabaseAuthorizer;
class DatabaseContext;
class ExecutionContext;
class SQLTransaction;
class SQLTransactionBackend;
class SQLTransactionCallback;
class SQLTransactionClient;
class SQLTransactionCoordinator;
class SQLTransactionErrorCallback;
class VoidCallback;

class Database final : public GarbageCollectedFinalized<Database>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~Database();
    DECLARE_TRACE();

    bool openAndVerifyVersion(bool setVersionInNewDatabase, DatabaseError&, String& errorMessage);
    void close();

    SQLTransactionBackend* runTransaction(SQLTransaction*, bool readOnly, const ChangeVersionData*);
    void scheduleTransactionStep(SQLTransactionBackend*);
    void inProgressTransactionCompleted();

    SQLTransactionClient* transactionClient() const;
    SQLTransactionCoordinator* transactionCoordinator() const;

    // Direct support for the DOM API
    String version() const;
    void changeVersion(
        const String& oldVersion,
        const String& newVersion,
        SQLTransactionCallback*,
        SQLTransactionErrorCallback*,
        VoidCallback* successCallback);
    void transaction(
        SQLTransactionCallback*,
        SQLTransactionErrorCallback*,
        VoidCallback* successCallback);
    void readTransaction(
        SQLTransactionCallback*,
        SQLTransactionErrorCallback*,
        VoidCallback* successCallback);

    bool opened() const { return m_opened; }
    bool isNew() const { return m_new; }

    SecurityOrigin* securityOrigin() const;
    String stringIdentifier() const;
    String displayName() const;
    unsigned long estimatedSize() const;
    String fileName() const;
    SQLiteDatabase& sqliteDatabase() { return m_sqliteDatabase; }

    unsigned long long maximumSize() const;
    void incrementalVacuumIfNeeded();

    void disableAuthorizer();
    void enableAuthorizer();
    void setAuthorizerPermissions(int);
    bool lastActionChangedDatabase();
    bool lastActionWasInsert();
    void resetDeletes();
    bool hadDeletes();
    void resetAuthorizer();

    Vector<String> tableNames();
    void scheduleTransactionCallback(SQLTransaction*);
    void closeImmediately();
    void closeDatabase();

    DatabaseContext* databaseContext() const { return m_databaseContext.get(); }
    ExecutionContext* executionContext() const;

private:
    class DatabaseOpenTask;
    class DatabaseCloseTask;
    class DatabaseTransactionTask;
    class DatabaseTableNamesTask;

    Database(DatabaseContext*, const String& name, const String& expectedVersion, const String& displayName, unsigned long estimatedSize);
    bool performOpenAndVerify(bool setVersionInNewDatabase, DatabaseError&, String& errorMessage);

    void scheduleTransaction();

    bool getVersionFromDatabase(String& version, bool shouldCacheVersion = true);
    bool setVersionInDatabase(const String& version, bool shouldCacheVersion = true);
    void setExpectedVersion(const String&);
    const String& expectedVersion() const { return m_expectedVersion; }
    String getCachedVersion()const;
    void setCachedVersion(const String&);
    bool getActualVersionForTransaction(String& version);

    void runTransaction(
        SQLTransactionCallback*,
        SQLTransactionErrorCallback*,
        VoidCallback* successCallback,
        bool readOnly,
        const ChangeVersionData* = 0);
    Vector<String> performGetTableNames();

    void reportOpenDatabaseResult(int errorSite, int webSqlErrorCode, int sqliteErrorCode, double duration);
    void reportChangeVersionResult(int errorSite, int webSqlErrorCode, int sqliteErrorCode);
    void reportStartTransactionResult(int errorSite, int webSqlErrorCode, int sqliteErrorCode);
    void reportCommitTransactionResult(int errorSite, int webSqlErrorCode, int sqliteErrorCode);
    void reportExecuteStatementResult(int errorSite, int webSqlErrorCode, int sqliteErrorCode);
    void reportVacuumDatabaseResult(int sqliteErrorCode);
    void logErrorMessage(const String&);
    static const char* databaseInfoTableName();
#if !LOG_DISABLED || !ERROR_DISABLED
    String databaseDebugName() const { return m_contextThreadSecurityOrigin->toString() + "::" + m_name; }
#endif

    RefPtr<SecurityOrigin> m_contextThreadSecurityOrigin;
    RefPtr<SecurityOrigin> m_databaseThreadSecurityOrigin;
    Member<DatabaseContext> m_databaseContext; // Associated with m_executionContext.

    String m_name;
    String m_expectedVersion;
    String m_displayName;
    unsigned long m_estimatedSize;
    String m_filename;

    DatabaseGuid m_guid;
    bool m_opened;
    bool m_new;

    SQLiteDatabase m_sqliteDatabase;

    Member<DatabaseAuthorizer> m_databaseAuthorizer;

    HeapDeque<Member<SQLTransactionBackend>> m_transactionQueue;
    Mutex m_transactionInProgressMutex;
    bool m_transactionInProgress;
    bool m_isTransactionQueueEnabled;

    friend class ChangeVersionWrapper;
    friend class DatabaseManager;
    friend class SQLStatementBackend;
    friend class SQLTransaction;
    friend class SQLTransactionBackend;
};

} // namespace blink

#endif // Database_h
