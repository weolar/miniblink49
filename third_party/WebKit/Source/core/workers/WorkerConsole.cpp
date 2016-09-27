/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "core/workers/WorkerConsole.h"

#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/workers/WorkerGlobalScope.h"
#include "core/workers/WorkerReportingProxy.h"
#include "core/workers/WorkerThread.h"


namespace blink {

WorkerConsole::WorkerConsole(WorkerGlobalScope* scope)
    : m_scope(scope)
{
}

WorkerConsole::~WorkerConsole()
{
}

void WorkerConsole::reportMessageToConsole(PassRefPtrWillBeRawPtr<ConsoleMessage> consoleMessage)
{
    if (RefPtrWillBeRawPtr<ScriptCallStack> callStack = consoleMessage->callStack()) {
        const ScriptCallFrame& lastCaller = callStack->at(0);
        consoleMessage->setURL(lastCaller.sourceURL());
        consoleMessage->setLineNumber(lastCaller.lineNumber());
    }
    m_scope->addConsoleMessage(consoleMessage);
}

ExecutionContext* WorkerConsole::context()
{
    if (!m_scope)
        return nullptr;
    return m_scope->executionContext();
}

DEFINE_TRACE(WorkerConsole)
{
    visitor->trace(m_scope);
    ConsoleBase::trace(visitor);
}

// FIXME: add memory getter

} // namespace blink
