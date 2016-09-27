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

#ifndef WorkerConsoleAgent_h
#define WorkerConsoleAgent_h

#include "core/inspector/InspectorConsoleAgent.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class WorkerGlobalScope;

class WorkerConsoleAgent final : public InspectorConsoleAgent {
    WTF_MAKE_NONCOPYABLE(WorkerConsoleAgent);
public:
    static PassOwnPtrWillBeRawPtr<WorkerConsoleAgent> create(InjectedScriptManager* injectedScriptManager, WorkerGlobalScope* workerGlobalScope)
    {
        return adoptPtrWillBeNoop(new WorkerConsoleAgent(injectedScriptManager, workerGlobalScope));
    }
    virtual ~WorkerConsoleAgent();
    DECLARE_VIRTUAL_TRACE();

    virtual void enable(ErrorString*) override;
    virtual void clearMessages(ErrorString*) override;

protected:
    virtual ConsoleMessageStorage* messageStorage() override;

    virtual void enableStackCapturingIfNeeded() override;
    virtual void disableStackCapturingIfNeeded() override;

private:
    WorkerConsoleAgent(InjectedScriptManager*, WorkerGlobalScope*);

    RawPtrWillBeMember<WorkerGlobalScope> m_workerGlobalScope;
};

} // namespace blink

#endif // !defined(WorkerConsoleAgent_h)
