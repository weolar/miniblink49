/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
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

#ifndef SQLiteStatement_h
#define SQLiteStatement_h

#include "modules/webdatabase/sqlite/SQLiteDatabase.h"

struct sqlite3_stmt;

namespace blink {

class SQLValue;

class SQLiteStatement {
    WTF_MAKE_NONCOPYABLE(SQLiteStatement); WTF_MAKE_FAST_ALLOCATED(SQLiteStatement);
public:
    SQLiteStatement(SQLiteDatabase&, const String&);
    ~SQLiteStatement();

    int prepare();
    int bindText(int index, const String&);
    int bindDouble(int index, double);
    int bindNull(int index);
    int bindValue(int index, const SQLValue&);
    unsigned bindParameterCount() const;

    int step();
    int finalize();

    int prepareAndStep() { if (int error = prepare()) return error; return step(); }

    // prepares, steps, and finalizes the query.
    // returns true if all 3 steps succeed with step() returning SQLITE_DONE
    // returns false otherwise
    bool executeCommand();

    // Returns -1 on last-step failing.  Otherwise, returns number of rows
    // returned in the last step()
    int columnCount();

    String getColumnName(int col);
    SQLValue getColumnValue(int col);
    String getColumnText(int col);
    int getColumnInt(int col);
    int64_t getColumnInt64(int col);

private:
    SQLiteDatabase& m_database;
    String m_query;
    sqlite3_stmt* m_statement;
#if ENABLE(ASSERT)
    bool m_isPrepared;
#endif
};

} // namespace blink

#endif // SQLiteStatement_h
