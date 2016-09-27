/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2011 Google, Inc. All Rights Reserved.
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
 *
 */

#include "config.h"
#include "modules/webdatabase/DatabaseContext.h"

#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseManager.h"
#include "modules/webdatabase/DatabaseTask.h"
#include "modules/webdatabase/DatabaseThread.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/Assertions.h"

namespace blink {

// How the DatabaseContext Life-Cycle works?
// ========================================
// ... in other words, who's keeping the DatabaseContext alive and how long does
// it need to stay alive?
//
// The DatabaseContext is referenced from:
// 1. DatabaseManager
// 2. Database
//
// At Birth:
// ========
// We create a DatabaseContext only when there is a need i.e. the script tries to
// open a Database via DatabaseManager::openDatabase().
//
// The DatabaseContext constructor will register itself to DatabaseManager. This
// lets DatabaseContext keep itself alive until it is unregisterd in
// contextDestroyed().
//
// Once a DatabaseContext is associated with a ExecutionContext, it will
// live until after the ExecutionContext destructs. This is true even if
// we don't succeed in opening any Databases for that context. When we do
// succeed in opening Databases for this ExecutionContext, the Database
// will re-use the same DatabaseContext.
//
// At Shutdown:
// ===========
// During shutdown, the DatabaseContext needs to:
// 1. "outlive" the ExecutionContext.
//    - This is needed because the DatabaseContext needs to remove itself from the
//      ExecutionContext's ActiveDOMObject list and ContextLifecycleObserver
//      list. This removal needs to be executed on the script's thread. Hence, we
//      rely on the ExecutionContext's shutdown process to call
//      stop() and contextDestroyed() to give us a chance to clean these up from
//      the script thread.
//
// 2. "outlive" the Databases.
//    - This is because they may make use of the DatabaseContext to execute a close
//      task and shutdown in an orderly manner. When the Databases are destructed,
//      they will release the DatabaseContext reference from the DatabaseThread.
//
// During shutdown, the ExecutionContext is shutting down on the script thread
// while the Databases are shutting down on the DatabaseThread. Hence, there can be
// a race condition as to whether the ExecutionContext or the Databases
// destruct first.
//
// The Members in the Databases and DatabaseManager will ensure that the
// DatabaseContext will outlive Database and ExecutionContext regardless of
// which of the 2 destructs first.

DatabaseContext* DatabaseContext::create(ExecutionContext* context)
{
    DatabaseContext* self = new DatabaseContext(context);
    DatabaseManager::manager().registerDatabaseContext(self);
    return self;
}

DatabaseContext::DatabaseContext(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_hasOpenDatabases(false)
    , m_hasRequestedTermination(false)
{
    // ActiveDOMObject expects this to be called to set internal flags.
    suspendIfNeeded();

    // For debug accounting only. We must do this before we register the
    // instance. The assertions assume this.
    DatabaseManager::manager().didConstructDatabaseContext();
}

DatabaseContext::~DatabaseContext()
{
    // For debug accounting only. We must call this last. The assertions assume
    // this.
    DatabaseManager::manager().didDestructDatabaseContext();
}

DEFINE_TRACE(DatabaseContext)
{
    visitor->trace(m_databaseThread);
    ActiveDOMObject::trace(visitor);
}

// This is called if the associated ExecutionContext is destructing while
// we're still associated with it. That's our cue to disassociate and shutdown.
// To do this, we stop the database and let everything shutdown naturally
// because the database closing process may still make use of this context.
// It is not safe to just delete the context here.
void DatabaseContext::contextDestroyed()
{
    stopDatabases();
    DatabaseManager::manager().unregisterDatabaseContext(this);
    ActiveDOMObject::contextDestroyed();
}

// stop() is from stopActiveDOMObjects() which indicates that the owner
// LocalFrame is shutting down. Initiate the orderly shutdown by stopping the
// associated databases.
void DatabaseContext::stop()
{
    stopDatabases();
}

DatabaseContext* DatabaseContext::backend()
{
    return this;
}

DatabaseThread* DatabaseContext::databaseThread()
{
    if (!m_databaseThread && !m_hasOpenDatabases) {
        // It's OK to ask for the m_databaseThread after we've requested
        // termination because we're still using it to execute the closing
        // of the database. However, it is NOT OK to create a new thread
        // after we've requested termination.
        ASSERT(!m_hasRequestedTermination);

        // Create the database thread on first request - but not if at least one database was already opened,
        // because in that case we already had a database thread and terminated it and should not create another.
        m_databaseThread = DatabaseThread::create();
        m_databaseThread->start();
    }

    return m_databaseThread.get();
}

bool DatabaseContext::databaseThreadAvailable()
{
    return databaseThread() && !m_hasRequestedTermination;
}

void DatabaseContext::stopDatabases()
{
    // Though we initiate termination of the DatabaseThread here in
    // stopDatabases(), we can't clear the m_databaseThread ref till we get to
    // the destructor. This is because the Databases that are managed by
    // DatabaseThread still rely on this ref between the context and the thread
    // to execute the task for closing the database. By the time we get to the
    // destructor, we're guaranteed that the databases are destructed (which is
    // why our ref count is 0 then and we're destructing). Then, the
    // m_databaseThread RefPtr destructor will deref and delete the
    // DatabaseThread.

    if (databaseThreadAvailable()) {
        m_hasRequestedTermination = true;
        // This blocks until the database thread finishes the cleanup task.
        m_databaseThread->terminate();
    }
}

bool DatabaseContext::allowDatabaseAccess() const
{
    return toDocument(executionContext())->isActive();
}

SecurityOrigin* DatabaseContext::securityOrigin() const
{
    return executionContext()->securityOrigin();
}

bool DatabaseContext::isContextThread() const
{
    return executionContext()->isContextThread();
}

} // namespace blink
