/*
 * Copyright (c) 2011 Google Inc. All rights reserved.
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

#include "config.h"
#include "core/inspector/MainThreadDebugger.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/WindowProxy.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameHost.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTaskRunner.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/ScriptDebugListener.h"
#include "core/page/Page.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/StdLibExtras.h"
#include "wtf/TemporaryChange.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static LocalFrame* retrieveFrameWithGlobalObjectCheck(v8::Local<v8::Context> context)
{
    return toLocalFrame(toFrameIfNotDetached(context));
}

// TODO(Oilpan): avoid keeping a raw reference separate from the
// owner one; does not enable heap-movable objects.
MainThreadDebugger* MainThreadDebugger::s_instance = nullptr;

MainThreadDebugger::MainThreadDebugger(PassOwnPtr<ClientMessageLoop> clientMessageLoop, v8::Isolate* isolate)
    : ScriptDebuggerBase(isolate, V8Debugger::create(isolate, this))
    , m_clientMessageLoop(clientMessageLoop)
    , m_pausedFrame(nullptr)
    , m_taskRunner(adoptPtr(new InspectorTaskRunner(isolate)))
{
    MutexLocker locker(creationMutex());
    ASSERT(!s_instance);
    s_instance = this;
}

MainThreadDebugger::~MainThreadDebugger()
{
    MutexLocker locker(creationMutex());
    ASSERT(s_instance == this);
    s_instance = nullptr;
}

Mutex& MainThreadDebugger::creationMutex()
{
    AtomicallyInitializedStaticReference(Mutex, mutex, (new Mutex));
    return mutex;
}

DEFINE_TRACE(MainThreadDebugger)
{
#if ENABLE(OILPAN)
    visitor->trace(m_pausedFrame);
    visitor->trace(m_listenersMap);
#endif
    ScriptDebuggerBase::trace(visitor);
}

void MainThreadDebugger::initializeContext(v8::Local<v8::Context> context, int worldId)
{
    LocalFrame* frame = retrieveFrameWithGlobalObjectCheck(context);
    if (!frame)
        return;
    LocalFrame* localFrameRoot = frame->localFrameRoot();
    String type = worldId == MainWorldId ? "page" : "injected";
    String debugData = "[" + type + "," + String::number(WeakIdentifierMap<LocalFrame>::identifier(localFrameRoot)) + "]";
    V8Debugger::setContextDebugData(context, debugData);
}

void MainThreadDebugger::addListener(ScriptDebugListener* listener, LocalFrame* localFrameRoot)
{
    ASSERT(localFrameRoot == localFrameRoot->localFrameRoot());

    ScriptController& scriptController = localFrameRoot->script();
    if (!scriptController.canExecuteScripts(NotAboutToExecuteScript))
        return;

    if (m_listenersMap.isEmpty())
        debugger()->enable();
    m_listenersMap.set(localFrameRoot, listener);
    String contextDataSubstring = "," + String::number(WeakIdentifierMap<LocalFrame>::identifier(localFrameRoot)) + "]";
    Vector<ScriptDebugListener::ParsedScript> compiledScripts;
    debugger()->getCompiledScripts(contextDataSubstring, compiledScripts);
    for (size_t i = 0; i < compiledScripts.size(); i++)
        listener->didParseSource(compiledScripts[i]);
}

void MainThreadDebugger::removeListener(ScriptDebugListener* listener, LocalFrame* localFrame)
{
    if (!m_listenersMap.contains(localFrame))
        return;

    if (m_pausedFrame == localFrame)
        debugger()->continueProgram();

    m_listenersMap.remove(localFrame);

    if (m_listenersMap.isEmpty())
        debugger()->disable();
}

MainThreadDebugger* MainThreadDebugger::instance()
{
    ASSERT(isMainThread());
    return s_instance;
}

void MainThreadDebugger::interruptMainThreadAndRun(PassOwnPtr<InspectorTaskRunner::Task> task)
{
    MutexLocker locker(creationMutex());
    if (s_instance)
        s_instance->m_taskRunner->interruptAndRun(task);
}

ScriptDebugListener* MainThreadDebugger::getDebugListenerForContext(v8::Local<v8::Context> context)
{
    v8::HandleScope scope(context->GetIsolate());
    LocalFrame* frame = retrieveFrameWithGlobalObjectCheck(context);
    if (!frame)
        return 0;
    return m_listenersMap.get(frame->localFrameRoot());
}

void MainThreadDebugger::runMessageLoopOnPause(v8::Local<v8::Context> context)
{
    v8::HandleScope scope(context->GetIsolate());
    LocalFrame* frame = retrieveFrameWithGlobalObjectCheck(context);
    m_pausedFrame = frame->localFrameRoot();

    // Wait for continue or step command.
    m_clientMessageLoop->run(m_pausedFrame);

    // The listener may have been removed in the nested loop.
    if (ScriptDebugListener* listener = m_listenersMap.get(m_pausedFrame))
        listener->didContinue();

    m_pausedFrame = 0;
}

void MainThreadDebugger::quitMessageLoopOnPause()
{
    m_clientMessageLoop->quitNow();
}

} // namespace blink
