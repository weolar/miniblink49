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

#ifndef DatabaseTracker_h
#define DatabaseTracker_h

#include "modules/ModulesExport.h"
#include "modules/webdatabase/DatabaseError.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Database;
class DatabaseContext;
class SecurityOrigin;

class MODULES_EXPORT DatabaseTracker {
    WTF_MAKE_NONCOPYABLE(DatabaseTracker); WTF_MAKE_FAST_ALLOCATED(DatabaseTracker);
public:
    static DatabaseTracker& tracker();
    // This singleton will potentially be used from multiple worker threads and the page's context thread simultaneously.  To keep this safe, it's
    // currently using 4 locks.  In order to avoid deadlock when taking multiple locks, you must take them in the correct order:
    // m_databaseGuard before quotaManager if both locks are needed.
    // m_openDatabaseMapGuard before quotaManager if both locks are needed.
    // m_databaseGuard and m_openDatabaseMapGuard currently don't overlap.
    // notificationMutex() is currently independent of the other locks.

    bool canEstablishDatabase(DatabaseContext*, const String& name, const String& displayName, unsigned long estimatedSize, DatabaseError&);
    String fullPathForDatabase(SecurityOrigin*, const String& name, bool createIfDoesNotExist = true);

    void addOpenDatabase(Database*);
    void removeOpenDatabase(Database*);

    unsigned long long getMaxSizeForDatabase(const Database*);

    void closeDatabasesImmediately(const String& originIdentifier, const String& name);

    void prepareToOpenDatabase(Database*);
    void failedToOpenDatabase(Database*);

private:
    typedef HashSet<Database*> DatabaseSet;
    typedef HashMap<String, DatabaseSet*> DatabaseNameMap;
    typedef HashMap<String, DatabaseNameMap*> DatabaseOriginMap;
    class CloseOneDatabaseImmediatelyTask;

    DatabaseTracker();

    void closeOneDatabaseImmediately(const String& originIdentifier, const String& name, Database*);

    Mutex m_openDatabaseMapGuard;
    // This map contains raw pointers to a garbage-collected class. We can't
    // make this traceable because it is updated by multiple database threads.
    GC_PLUGIN_IGNORE("crbug.com/417990")
    mutable OwnPtr<DatabaseOriginMap> m_openDatabaseMap;
};

} // namespace blink

#endif // DatabaseTracker_h
