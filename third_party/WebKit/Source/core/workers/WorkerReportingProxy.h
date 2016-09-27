/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef WorkerReportingProxy_h
#define WorkerReportingProxy_h

#include "core/CoreExport.h"
#include "core/frame/ConsoleTypes.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class ConsoleMessage;
class WorkerGlobalScope;

// APIs used by workers to report console and worker activity.
class CORE_EXPORT WorkerReportingProxy {
public:
    virtual ~WorkerReportingProxy() { }

    virtual void reportException(const String& errorMessage, int lineNumber, int columnNumber, const String& sourceURL, int exceptionId) = 0;
    virtual void reportConsoleMessage(PassRefPtrWillBeRawPtr<ConsoleMessage>) = 0;
    virtual void postMessageToPageInspector(const String&) = 0;
    virtual void postWorkerConsoleAgentEnabled() = 0;

    // Invoked when the worker script is evaluated. |success| is true if the
    // evaluation completed with no uncaught exception.
    virtual void didEvaluateWorkerScript(bool success) = 0;

    // Invoked when the new WorkerGlobalScope is started.
    virtual void workerGlobalScopeStarted(WorkerGlobalScope*) = 0;

    // Invoked when close() is invoked on the worker context.
    virtual void workerGlobalScopeClosed() = 0;

    // Invoked when the thread is stopped and WorkerGlobalScope is being
    // destructed. (This is be the last method that is called on this
    // interface)
    virtual void workerThreadTerminated() = 0;

    // Invoked when the thread is about to be stopped and WorkerGlobalScope
    // is to be destructed. (When this is called it is guaranteed that
    // WorkerGlobalScope is still alive)
    virtual void willDestroyWorkerGlobalScope() = 0;
};

} // namespace blink

#endif // WorkerReportingProxy_h
