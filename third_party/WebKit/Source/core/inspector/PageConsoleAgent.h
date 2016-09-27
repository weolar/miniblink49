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

#ifndef PageConsoleAgent_h
#define PageConsoleAgent_h

#include "core/CoreExport.h"
#include "core/inspector/InspectorConsoleAgent.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ConsoleMessage;
class ConsoleMessageStorage;
class InspectorDOMAgent;
class InspectorPageAgent;
class WorkerInspectorProxy;
class WorkerGlobalScopeProxy;

class CORE_EXPORT PageConsoleAgent final : public InspectorConsoleAgent {
    WTF_MAKE_NONCOPYABLE(PageConsoleAgent);
public:
    static PassOwnPtrWillBeRawPtr<PageConsoleAgent> create(InjectedScriptManager* injectedScriptManager, InspectorDOMAgent* domAgent, InspectorPageAgent* pageAgent)
    {
        return adoptPtrWillBeNoop(new PageConsoleAgent(injectedScriptManager, domAgent, pageAgent));
    }
    virtual ~PageConsoleAgent();
    DECLARE_VIRTUAL_TRACE();

    virtual void enable(ErrorString*) override;
    virtual void disable(ErrorString*) override;

    void workerTerminated(WorkerInspectorProxy*);

    void workerConsoleAgentEnabled(WorkerGlobalScopeProxy*);

protected:
    virtual ConsoleMessageStorage* messageStorage() override;

    virtual void enableStackCapturingIfNeeded() override;
    virtual void disableStackCapturingIfNeeded() override;

private:
    PageConsoleAgent(InjectedScriptManager*, InspectorDOMAgent*, InspectorPageAgent*);
    virtual void clearMessages(ErrorString*) override;

    RawPtrWillBeMember<InspectorDOMAgent> m_inspectorDOMAgent;
    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
    HashSet<WorkerGlobalScopeProxy*> m_workersWithEnabledConsole;

    static int s_enabledAgentCount;
};

} // namespace blink


#endif // !defined(PageConsoleAgent_h)
