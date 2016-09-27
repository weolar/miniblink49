/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef DatabaseClient_h
#define DatabaseClient_h

#include "core/page/Page.h"
#include "modules/ModulesExport.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class Database;
class ExecutionContext;
class InspectorDatabaseAgent;
class Page;

class MODULES_EXPORT DatabaseClient : public WillBeHeapSupplement<Page> {
    WTF_MAKE_NONCOPYABLE(DatabaseClient);
public:
    DatabaseClient();
    virtual ~DatabaseClient() { }

    virtual bool allowDatabase(ExecutionContext*, const String& name, const String& displayName, unsigned long estimatedSize) = 0;

    void didOpenDatabase(Database*, const String& domain, const String& name, const String& version);

    static DatabaseClient* fromPage(Page*);
    static DatabaseClient* from(ExecutionContext*);
    static const char* supplementName();

    void setInspectorAgent(InspectorDatabaseAgent*);

private:
    InspectorDatabaseAgent* m_inspectorAgent;
};

MODULES_EXPORT void provideDatabaseClientTo(Page&, PassOwnPtrWillBeRawPtr<DatabaseClient>);

} // namespace blink

#endif // DatabaseClient_h
