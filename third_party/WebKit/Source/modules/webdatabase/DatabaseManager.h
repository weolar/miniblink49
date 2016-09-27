/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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

#ifndef DatabaseManager_h
#define DatabaseManager_h

#include "modules/webdatabase/DatabaseContext.h"
#include "modules/webdatabase/DatabaseError.h"
#include "platform/heap/Handle.h"
#include "wtf/Assertions.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"

namespace blink {

class Database;
class DatabaseCallback;
class DatabaseContext;
class ExceptionState;
class SecurityOrigin;
class ExecutionContext;

class DatabaseManager {
    WTF_MAKE_NONCOPYABLE(DatabaseManager); WTF_MAKE_FAST_ALLOCATED(DatabaseManager);
public:
    static DatabaseManager& manager();
    static void terminateDatabaseThread();

    // These 2 methods are for DatabaseContext (un)registration, and should only
    // be called by the DatabaseContext constructor and destructor.
    void registerDatabaseContext(DatabaseContext*);
    void unregisterDatabaseContext(DatabaseContext*);

#if ENABLE(ASSERT)
    void didConstructDatabaseContext();
    void didDestructDatabaseContext();
#else
    void didConstructDatabaseContext() { }
    void didDestructDatabaseContext() { }
#endif

    static void throwExceptionForDatabaseError(DatabaseError, const String& errorMessage, ExceptionState&);

    Database* openDatabase(ExecutionContext*, const String& name, const String& expectedVersion, const String& displayName, unsigned long estimatedSize, DatabaseCallback*, DatabaseError&, String& errorMessage);

    String fullPathForDatabase(SecurityOrigin*, const String& name, bool createIfDoesNotExist = true);

private:
    DatabaseManager();
    ~DatabaseManager();

    // This gets a DatabaseContext for the specified ExecutionContext.
    // If one doesn't already exist, it will create a new one.
    DatabaseContext* databaseContextFor(ExecutionContext*);
    // This gets a DatabaseContext for the specified ExecutionContext if
    // it already exist previously. Otherwise, it returns 0.
    DatabaseContext* existingDatabaseContextFor(ExecutionContext*);

    Database* openDatabaseInternal(ExecutionContext*,
        const String& name, const String& expectedVersion, const String& displayName,
        unsigned long estimatedSize, bool setVersionInNewDatabase, DatabaseError&, String& errorMessage);

    static void logErrorMessage(ExecutionContext*, const String& message);

    // m_contextMap can have two or more entries even though we don't support
    // Web SQL on workers because single Blink process can have multiple main
    // contexts.
    typedef PersistentHeapHashMap<ExecutionContext*, Member<DatabaseContext>> ContextMap;
    ContextMap m_contextMap;
#if ENABLE(ASSERT)
    int m_databaseContextRegisteredCount;
    int m_databaseContextInstanceCount;
#endif
};

} // namespace blink

#endif // DatabaseManager_h
