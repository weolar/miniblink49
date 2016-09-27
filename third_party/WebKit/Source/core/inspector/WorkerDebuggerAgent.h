/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef WorkerDebuggerAgent_h
#define WorkerDebuggerAgent_h

#include "core/inspector/InspectorDebuggerAgent.h"

namespace blink {

class WorkerGlobalScope;
class WorkerThreadDebugger;

class WorkerDebuggerAgent final : public InspectorDebuggerAgent {
    WTF_MAKE_NONCOPYABLE(WorkerDebuggerAgent);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(WorkerDebuggerAgent);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(WorkerDebuggerAgent);
public:
    static PassOwnPtrWillBeRawPtr<WorkerDebuggerAgent> create(WorkerThreadDebugger*, WorkerGlobalScope*, InjectedScriptManager*);
    ~WorkerDebuggerAgent() override;
    DECLARE_VIRTUAL_TRACE();

private:

    WorkerDebuggerAgent(WorkerThreadDebugger*, WorkerGlobalScope*, InjectedScriptManager*);

    void startListeningV8Debugger() override;
    void stopListeningV8Debugger() override;
    V8Debugger& debugger() override;
    InjectedScript injectedScriptForEval(ErrorString*, const int* executionContextId) override;
    void muteConsole() override;
    void unmuteConsole() override;

    WorkerThreadDebugger* m_workerThreadDebugger;
    RawPtrWillBeMember<WorkerGlobalScope> m_inspectedWorkerGlobalScope;
};

} // namespace blink

#endif // !defined(WorkerDebuggerAgent_h)
