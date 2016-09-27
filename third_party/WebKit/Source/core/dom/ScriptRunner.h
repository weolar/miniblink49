/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
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

#ifndef ScriptRunner_h
#define ScriptRunner_h

#include "core/CoreExport.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/heap/Handle.h"
#include "platform/scheduler/CancellableTaskFactory.h"
#include "wtf/Deque.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class Document;
class ScriptLoader;

class CORE_EXPORT ScriptRunner final : public NoBaseWillBeGarbageCollectedFinalized<ScriptRunner> {
    WTF_MAKE_NONCOPYABLE(ScriptRunner); WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(ScriptRunner);
public:
    static PassOwnPtrWillBeRawPtr<ScriptRunner> create(Document* document)
    {
        return adoptPtrWillBeNoop(new ScriptRunner(document));
    }
    ~ScriptRunner();

    enum ExecutionType { ASYNC_EXECUTION, IN_ORDER_EXECUTION };
    void queueScriptForExecution(ScriptLoader*, ExecutionType);
    bool hasPendingScripts() const { return !m_scriptsToExecuteSoon.isEmpty() || !m_scriptsToExecuteInOrder.isEmpty() || !m_pendingAsyncScripts.isEmpty(); }
    void suspend();
    void resume();
    void notifyScriptReady(ScriptLoader*, ExecutionType);
    void notifyScriptLoadError(ScriptLoader*, ExecutionType);

    static void movePendingAsyncScript(Document&, Document&, ScriptLoader*);

    DECLARE_TRACE();

private:
    explicit ScriptRunner(Document*);

    void executeScripts();

    void addPendingAsyncScript(ScriptLoader*);

    void movePendingAsyncScript(ScriptRunner*, ScriptLoader*);

    bool yieldForHighPriorityWork();

    void postTaskIfOneIsNotAlreadyInFlight();

    RawPtrWillBeMember<Document> m_document;
    WillBeHeapDeque<RawPtrWillBeMember<ScriptLoader>> m_scriptsToExecuteInOrder;
    // http://www.whatwg.org/specs/web-apps/current-work/#set-of-scripts-that-will-execute-as-soon-as-possible
    WillBeHeapDeque<RawPtrWillBeMember<ScriptLoader>> m_scriptsToExecuteSoon;
    WillBeHeapHashSet<RawPtrWillBeMember<ScriptLoader>> m_pendingAsyncScripts;
    CancellableTaskFactory m_executeScriptsTaskFactory;
};

}

#endif
