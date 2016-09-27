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

#ifndef WorkerThreadStartupData_h
#define WorkerThreadStartupData_h

#include "bindings/core/v8/V8CacheOptions.h"
#include "core/CoreExport.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/workers/WorkerClients.h"
#include "core/workers/WorkerThread.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"

namespace blink {

class WorkerClients;

class CORE_EXPORT WorkerThreadStartupData final {
    WTF_MAKE_NONCOPYABLE(WorkerThreadStartupData);
    WTF_MAKE_FAST_ALLOCATED(WorkerThreadStartupData);
public:
    static PassOwnPtr<WorkerThreadStartupData> create(const KURL& scriptURL, const String& userAgent, const String& sourceCode, PassOwnPtr<Vector<char>> cachedMetaData, WorkerThreadStartMode startMode, const PassOwnPtr<Vector<CSPHeaderAndType>> contentSecurityPolicyHeaders, const SecurityOrigin* starterOrigin, PassOwnPtrWillBeRawPtr<WorkerClients> workerClients, V8CacheOptions v8CacheOptions = V8CacheOptionsDefault)
    {
        return adoptPtr(new WorkerThreadStartupData(scriptURL, userAgent, sourceCode, cachedMetaData, startMode, contentSecurityPolicyHeaders, starterOrigin, workerClients, v8CacheOptions));
    }

    ~WorkerThreadStartupData();

    KURL m_scriptURL;
    String m_userAgent;
    String m_sourceCode;
    OwnPtr<Vector<char>> m_cachedMetaData;
    WorkerThreadStartMode m_startMode;
    OwnPtr<Vector<CSPHeaderAndType>> m_contentSecurityPolicyHeaders;

    // The SecurityOrigin of the Document creating a Worker may have
    // been configured with extra policy privileges when it was created
    // (e.g., enforce path-based file:// origins.)
    // To ensure that these are transferred to the origin of a new worker
    // global scope, supply the Document's SecurityOrigin as the
    // 'starter origin'.
    //
    // Ownership of this optional starter origin remain with the caller,
    // and is assumed to stay alive until the new Worker thread has been
    // initialized.
    //
    // See SecurityOrigin::transferPrivilegesFrom() for details on what
    // privileges are transferred.
    const SecurityOrigin* m_starterOrigin;

    // This object is created and initialized on the thread creating
    // a new worker context, but ownership of it and this WorkerThreadStartupData
    // structure is passed along to the new worker thread, where it is finalized.
    //
    // Hence, CrossThreadPersistent<> is required to allow finalization
    // to happen on a thread different than the thread creating the
    // persistent reference. If the worker thread creation context
    // supplies no extra 'clients', m_workerClients can be left as empty/null.
    OwnPtrWillBeCrossThreadPersistent<WorkerClients> m_workerClients;

    V8CacheOptions m_v8CacheOptions;

private:
    WorkerThreadStartupData(const KURL& scriptURL, const String& userAgent, const String& sourceCode, PassOwnPtr<Vector<char>> cachedMetaData, WorkerThreadStartMode, const PassOwnPtr<Vector<CSPHeaderAndType>> contentSecurityPolicyHeaders, const SecurityOrigin*, PassOwnPtrWillBeRawPtr<WorkerClients>, V8CacheOptions);
};

} // namespace blink

#endif // WorkerThreadStartupData_h
