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

#include "config.h"
#include "modules/webdatabase/DatabaseThread.h"

#include "modules/webdatabase/Database.h"
#include "modules/webdatabase/DatabaseTask.h"
#include "modules/webdatabase/SQLTransactionClient.h"
#include "modules/webdatabase/SQLTransactionCoordinator.h"
#include "platform/Logging.h"
#include "platform/ThreadSafeFunctional.h"
#include "platform/heap/glue/MessageLoopInterruptor.h"
#include "platform/heap/glue/PendingGCRunner.h"
#include "public/platform/Platform.h"

namespace blink {

DatabaseThread::DatabaseThread()
    : m_transactionClient(adoptPtr(new SQLTransactionClient()))
    , m_transactionCoordinator(new SQLTransactionCoordinator())
    , m_cleanupSync(0)
    , m_terminationRequested(false)
{
}

DatabaseThread::~DatabaseThread()
{
    ASSERT(m_openDatabaseSet.isEmpty());
    ASSERT(!m_thread);
}

DEFINE_TRACE(DatabaseThread)
{
    visitor->trace(m_openDatabaseSet);
    visitor->trace(m_transactionCoordinator);
}

void DatabaseThread::start()
{
    if (m_thread)
        return;
    m_thread = WebThreadSupportingGC::create("WebCore: Database");
    m_thread->postTask(FROM_HERE, new Task(threadSafeBind(&DatabaseThread::setupDatabaseThread, this)));
}

void DatabaseThread::setupDatabaseThread()
{
    m_thread->initialize();
}

void DatabaseThread::terminate()
{
    TaskSynchronizer sync;
    {
        MutexLocker lock(m_terminationRequestedMutex);
        ASSERT(!m_terminationRequested);
        m_terminationRequested = true;
        m_cleanupSync = &sync;
        WTF_LOG(StorageAPI, "DatabaseThread %p was asked to terminate\n", this);
        m_thread->postTask(FROM_HERE, new Task(threadSafeBind(&DatabaseThread::cleanupDatabaseThread, this)));
    }
    sync.waitForTaskCompletion();
    // The WebThread destructor blocks until all the tasks of the database
    // thread are processed. However, it shouldn't block at all because
    // the database thread has already finished processing the cleanup task.
    m_thread.clear();
}

bool DatabaseThread::terminationRequested() const
{
    MutexLocker lock(m_terminationRequestedMutex);
    return m_terminationRequested;
}

void DatabaseThread::cleanupDatabaseThread()
{
    WTF_LOG(StorageAPI, "Cleaning up DatabaseThread %p", this);

    // Clean up the list of all pending transactions on this database thread
    m_transactionCoordinator->shutdown();

    // Close the databases that we ran transactions on. This ensures that if any transactions are still open, they are rolled back and we don't leave the database in an
    // inconsistent or locked state.
    if (m_openDatabaseSet.size() > 0) {
        // As the call to close will modify the original set, we must take a copy to iterate over.
        HeapHashSet<Member<Database>> openSetCopy;
        openSetCopy.swap(m_openDatabaseSet);
        HeapHashSet<Member<Database>>::iterator end = openSetCopy.end();
        for (HeapHashSet<Member<Database>>::iterator it = openSetCopy.begin(); it != end; ++it)
            (*it)->close();
    }
    m_openDatabaseSet.clear();

    m_thread->postTask(FROM_HERE, new Task(WTF::bind(&DatabaseThread::cleanupDatabaseThreadCompleted, this)));
}

void DatabaseThread::cleanupDatabaseThreadCompleted()
{
    m_thread->shutdown();
    if (m_cleanupSync) // Someone wanted to know when we were done cleaning up.
        m_cleanupSync->taskCompleted();
}

void DatabaseThread::recordDatabaseOpen(Database* database)
{
    ASSERT(isDatabaseThread());
    ASSERT(database);
    ASSERT(!m_openDatabaseSet.contains(database));
    MutexLocker lock(m_terminationRequestedMutex);
    if (!m_terminationRequested)
        m_openDatabaseSet.add(database);
}

void DatabaseThread::recordDatabaseClosed(Database* database)
{
    ASSERT(isDatabaseThread());
    ASSERT(database);
    ASSERT(m_terminationRequested || m_openDatabaseSet.contains(database));
    m_openDatabaseSet.remove(database);
}

bool DatabaseThread::isDatabaseOpen(Database* database)
{
    ASSERT(isDatabaseThread());
    ASSERT(database);
    MutexLocker lock(m_terminationRequestedMutex);
    return !m_terminationRequested && m_openDatabaseSet.contains(database);
}

void DatabaseThread::scheduleTask(PassOwnPtr<DatabaseTask> task)
{
    ASSERT(m_thread);
    ASSERT(!terminationRequested());
    // WebThread takes ownership of the task.
    m_thread->postTask(FROM_HERE, new Task(threadSafeBind(&DatabaseTask::run, task)));
}

} // namespace blink
