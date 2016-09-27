/*
 * Copyright (C) 2007, 2008, 2013 Apple Inc. All rights reserved.
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

#ifndef DatabaseTask_h
#define DatabaseTask_h

#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseBasicTypes.h"
#include "modules/webdatabase/DatabaseError.h"
#include "modules/webdatabase/SQLTransactionBackend.h"
#include "platform/Task.h"
#include "platform/TaskSynchronizer.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Threading.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DatabaseTask {
    WTF_MAKE_NONCOPYABLE(DatabaseTask); WTF_MAKE_FAST_ALLOCATED(DatabaseTask);
public:
    virtual ~DatabaseTask();

    void run();

    Database* database() const { return m_database.get(); }
#if ENABLE(ASSERT)
    bool hasSynchronizer() const { return m_synchronizer; }
#endif

protected:
    DatabaseTask(Database*, TaskSynchronizer*);

private:
    virtual void doPerformTask() = 0;
    virtual void taskCancelled() { }

    CrossThreadPersistent<Database> m_database;
    TaskSynchronizer* m_synchronizer;

#if !LOG_DISABLED
    virtual const char* debugTaskName() const = 0;
    bool m_complete;
#endif
};

class Database::DatabaseOpenTask final : public DatabaseTask {
public:
    static PassOwnPtr<DatabaseOpenTask> create(Database* db, bool setVersionInNewDatabase, TaskSynchronizer* synchronizer, DatabaseError& error, String& errorMessage, bool& success)
    {
        return adoptPtr(new DatabaseOpenTask(db, setVersionInNewDatabase, synchronizer, error, errorMessage, success));
    }

private:
    DatabaseOpenTask(Database*, bool setVersionInNewDatabase, TaskSynchronizer*, DatabaseError&, String& errorMessage, bool& success);

    void doPerformTask() override;
#if !LOG_DISABLED
    const char* debugTaskName() const override;
#endif

    bool m_setVersionInNewDatabase;
    DatabaseError& m_error;
    String& m_errorMessage;
    bool& m_success;
};

class Database::DatabaseCloseTask final : public DatabaseTask {
public:
    static PassOwnPtr<DatabaseCloseTask> create(Database* db, TaskSynchronizer* synchronizer)
    {
        return adoptPtr(new DatabaseCloseTask(db, synchronizer));
    }

private:
    DatabaseCloseTask(Database*, TaskSynchronizer*);

    void doPerformTask() override;
#if !LOG_DISABLED
    const char* debugTaskName() const override;
#endif
};

class Database::DatabaseTransactionTask final : public DatabaseTask {
public:
    ~DatabaseTransactionTask() override;

    // Transaction task is never synchronous, so no 'synchronizer' parameter.
    static PassOwnPtr<DatabaseTransactionTask> create(SQLTransactionBackend* transaction)
    {
        return adoptPtr(new DatabaseTransactionTask(transaction));
    }

    SQLTransactionBackend* transaction() const { return m_transaction.get(); }

private:
    explicit DatabaseTransactionTask(SQLTransactionBackend*);

    void doPerformTask() override;
    void taskCancelled() override;
#if !LOG_DISABLED
    const char* debugTaskName() const override;
#endif

    CrossThreadPersistent<SQLTransactionBackend> m_transaction;
};

class Database::DatabaseTableNamesTask final : public DatabaseTask {
public:
    static PassOwnPtr<DatabaseTableNamesTask> create(Database* db, TaskSynchronizer* synchronizer, Vector<String>& names)
    {
        return adoptPtr(new DatabaseTableNamesTask(db, synchronizer, names));
    }

private:
    DatabaseTableNamesTask(Database*, TaskSynchronizer*, Vector<String>& names);

    void doPerformTask() override;
#if !LOG_DISABLED
    const char* debugTaskName() const override;
#endif

    Vector<String>& m_tableNames;
};

} // namespace blink

#endif // DatabaseTask_h
