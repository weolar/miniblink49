/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Justin Haygood (jhaygood@reaktix.com)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLiteDatabase_h
#define SQLiteDatabase_h

#include "platform/heap/Handle.h"
#include "wtf/Threading.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"

#if COMPILER(MSVC)
#pragma warning(disable: 4800)
#endif

struct sqlite3;

namespace blink {

class DatabaseAuthorizer;
class SQLiteTransaction;

extern const int SQLResultDone;
extern const int SQLResultOk;
extern const int SQLResultRow;
extern const int SQLResultFull;
extern const int SQLResultInterrupt;
extern const int SQLResultConstraint;

class SQLiteDatabase {
    DISALLOW_ALLOCATION();
    WTF_MAKE_NONCOPYABLE(SQLiteDatabase);
    friend class SQLiteTransaction;
public:
    SQLiteDatabase();
    ~SQLiteDatabase();

    bool open(const String& filename);
    bool isOpen() const { return m_db; }
    void close();

    void updateLastChangesCount();

    bool executeCommand(const String&);

    bool tableExists(const String&);
    int runVacuumCommand();
    int runIncrementalVacuumCommand();

    bool transactionInProgress() const { return m_transactionInProgress; }

    int64_t lastInsertRowID();
    int lastChanges();

    void setBusyTimeout(int ms);

    // Sets the maximum size in bytes
    // Depending on per-database attributes, the size will only be settable in units that are the page size of the database, which is established at creation
    // These chunks will never be anything other than 512, 1024, 2048, 4096, 8192, 16384, or 32768 bytes in size.
    // setMaximumSize() will round the size down to the next smallest chunk if the passed size doesn't align.
    void setMaximumSize(int64_t);

    // Gets the number of unused bytes in the database file.
    int64_t freeSpaceSize();
    int64_t totalSize();

    int lastError();
    const char* lastErrorMsg();

    sqlite3* sqlite3Handle() const {
        ASSERT(m_sharable || currentThread() == m_openingThread || !m_db);
        return m_db;
    }

    void setAuthorizer(DatabaseAuthorizer*);

    bool isAutoCommitOn() const;

    // The SQLite AUTO_VACUUM pragma can be either NONE, FULL, or INCREMENTAL.
    // NONE - SQLite does not do any vacuuming
    // FULL - SQLite moves all empty pages to the end of the DB file and truncates
    //        the file to remove those pages after every transaction. This option
    //        requires SQLite to store additional information about each page in
    //        the database file.
    // INCREMENTAL - SQLite stores extra information for each page in the database
    //               file, but removes the empty pages only when PRAGMA INCREMANTAL_VACUUM
    //               is called.
    enum AutoVacuumPragma { AutoVacuumNone = 0, AutoVacuumFull = 1, AutoVacuumIncremental = 2 };
    bool turnOnIncrementalAutoVacuum();

    DECLARE_TRACE();

private:
    static int authorizerFunction(void*, int, const char*, const char*, const char*, const char*);

    void enableAuthorizer(bool enable);

    int pageSize();

    sqlite3* m_db;
    int m_pageSize;

    bool m_transactionInProgress;
    bool m_sharable;

    Mutex m_authorizerLock;
    Member<DatabaseAuthorizer> m_authorizer;

    ThreadIdentifier m_openingThread;

    Mutex m_databaseClosingMutex;

    int m_openError;
    CString m_openErrorMessage;

    int m_lastChangesCount;
};

} // namespace blink

#endif
