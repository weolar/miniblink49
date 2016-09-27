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

#include "config.h"
#include "core/dom/ScriptRunner.h"

#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ScriptLoader.h"
#include "platform/heap/Handle.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScheduler.h"
#include "public/platform/WebThread.h"
#include "wtf/Functional.h"

// This bit of magic is needed by oilpan to prevent the ScriptRunner from leaking.
namespace WTF {
template<>
struct ParamStorageTraits<blink::ScriptRunner*> : public PointerParamStorageTraits<blink::ScriptRunner*, false> {
};
}

namespace blink {


ScriptRunner::ScriptRunner(Document* document)
    : m_document(document)
    , m_executeScriptsTaskFactory(WTF::bind(&ScriptRunner::executeScripts, this))
{
    ASSERT(document);
#if ENABLE(LAZY_SWEEPING) && defined(ADDRESS_SANITIZER)
    m_executeScriptsTaskFactory.setUnpoisonBeforeUpdate();
#endif
}

ScriptRunner::~ScriptRunner()
{
#if !ENABLE(OILPAN)
    // Make sure that ScriptLoaders don't keep their PendingScripts alive.
    for (ScriptLoader* scriptLoader : m_scriptsToExecuteInOrder)
        scriptLoader->detach();
    for (ScriptLoader* scriptLoader : m_scriptsToExecuteSoon)
        scriptLoader->detach();
    for (ScriptLoader* scriptLoader : m_pendingAsyncScripts)
        scriptLoader->detach();
#endif
}

void ScriptRunner::addPendingAsyncScript(ScriptLoader* scriptLoader)
{
    m_document->incrementLoadEventDelayCount();
    m_pendingAsyncScripts.add(scriptLoader);
}

void ScriptRunner::queueScriptForExecution(ScriptLoader* scriptLoader, ExecutionType executionType)
{
    ASSERT(scriptLoader);

    switch (executionType) {
    case ASYNC_EXECUTION:
        addPendingAsyncScript(scriptLoader);
        break;

    case IN_ORDER_EXECUTION:
        m_document->incrementLoadEventDelayCount();
        m_scriptsToExecuteInOrder.append(scriptLoader);
        break;
    }
}

void ScriptRunner::suspend()
{
    m_executeScriptsTaskFactory.cancel();
}

void ScriptRunner::resume()
{
    if (hasPendingScripts())
        postTaskIfOneIsNotAlreadyInFlight();
}

void ScriptRunner::notifyScriptReady(ScriptLoader* scriptLoader, ExecutionType executionType)
{
    RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(scriptLoader);
    switch (executionType) {
    case ASYNC_EXECUTION:
        // RELEASE_ASSERT makes us crash in a controlled way in error cases
        // where the ScriptLoader is associated with the wrong ScriptRunner
        // (otherwise we'd cause a use-after-free in ~ScriptRunner when it tries
        // to detach).
        RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(m_pendingAsyncScripts.contains(scriptLoader));
        m_scriptsToExecuteSoon.append(scriptLoader);
        m_pendingAsyncScripts.remove(scriptLoader);
        break;

    case IN_ORDER_EXECUTION:
        RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(!m_scriptsToExecuteInOrder.isEmpty());
        break;
    }
    postTaskIfOneIsNotAlreadyInFlight();
}

void ScriptRunner::notifyScriptLoadError(ScriptLoader* scriptLoader, ExecutionType executionType)
{
    switch (executionType) {
    case ASYNC_EXECUTION:
        // RELEASE_ASSERT makes us crash in a controlled way in error cases
        // where the ScriptLoader is associated with the wrong ScriptRunner
        // (otherwise we'd cause a use-after-free in ~ScriptRunner when it tries
        // to detach).
        RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(m_pendingAsyncScripts.contains(scriptLoader));
        m_pendingAsyncScripts.remove(scriptLoader);
        scriptLoader->detach();
        m_document->decrementLoadEventDelayCount();
        break;

    case IN_ORDER_EXECUTION:
        RELEASE_ASSERT_WITH_SECURITY_IMPLICATION(!m_scriptsToExecuteInOrder.isEmpty());
        break;
    }
}

void ScriptRunner::movePendingAsyncScript(Document& oldDocument, Document& newDocument, ScriptLoader* scriptLoader)
{
    RefPtrWillBeRawPtr<Document> newContextDocument = newDocument.contextDocument().get();
    if (!newContextDocument) {
        // Document's contextDocument() method will return no Document if the
        // following conditions both hold:
        //
        //   - The Document wasn't created with an explicit context document
        //     and that document is otherwise kept alive.
        //   - The Document itself is detached from its frame.
        //
        // The script element's loader is in that case moved to document() and
        // its script runner, which is the non-null Document that contextDocument()
        // would return if not detached.
        ASSERT(!newDocument.frame());
        newContextDocument = &newDocument;
    }
    RefPtrWillBeRawPtr<Document> oldContextDocument = oldDocument.contextDocument().get();
    if (!oldContextDocument) {
        ASSERT(!oldDocument.frame());
        oldContextDocument = &oldDocument;
    }
    if (oldContextDocument != newContextDocument)
        oldContextDocument->scriptRunner()->movePendingAsyncScript(newContextDocument->scriptRunner(), scriptLoader);
}

void ScriptRunner::movePendingAsyncScript(ScriptRunner* newRunner, ScriptLoader* scriptLoader)
{
    if (m_pendingAsyncScripts.contains(scriptLoader)) {
        newRunner->addPendingAsyncScript(scriptLoader);
        m_pendingAsyncScripts.remove(scriptLoader);
        m_document->decrementLoadEventDelayCount();
    }
}

void ScriptRunner::executeScripts()
{
    RefPtrWillBeRawPtr<Document> protect(m_document.get());

    WillBeHeapDeque<RawPtrWillBeMember<ScriptLoader>> scriptLoaders;
    scriptLoaders.swap(m_scriptsToExecuteSoon);

    WillBeHeapHashSet<RawPtrWillBeMember<ScriptLoader>> inorderSet;
    while (!m_scriptsToExecuteInOrder.isEmpty() && m_scriptsToExecuteInOrder.first()->isReady()) {
        ScriptLoader* script = m_scriptsToExecuteInOrder.takeFirst();
        inorderSet.add(script);
        scriptLoaders.append(script);
    }

    while (!scriptLoaders.isEmpty()) {
        scriptLoaders.takeFirst()->execute();
        m_document->decrementLoadEventDelayCount();

        if (yieldForHighPriorityWork())
            break;
    }

    // If we have to yield, we must re-enqueue any scriptLoaders back onto the front of
    // m_scriptsToExecuteInOrder or m_scriptsToExecuteSoon depending on where the script
    // came from.
    // NOTE a yield followed by a notifyScriptReady(... ASYNC_EXECUTION) will result in that script executing
    // before any pre-existing ScriptsToExecuteInOrder.
    while (!scriptLoaders.isEmpty()) {
        ScriptLoader* script = scriptLoaders.takeLast();
        if (inorderSet.contains(script))
            m_scriptsToExecuteInOrder.prepend(script);
        else
            m_scriptsToExecuteSoon.prepend(script);
    }
}

bool ScriptRunner::yieldForHighPriorityWork()
{
    if (!Platform::current()->currentThread()->scheduler()->shouldYieldForHighPriorityWork())
        return false;

    postTaskIfOneIsNotAlreadyInFlight();
    return true;
}

void ScriptRunner::postTaskIfOneIsNotAlreadyInFlight()
{
    if (m_executeScriptsTaskFactory.isPending())
        return;

    // FIXME: Rename task() so that it's obvious it cancels any pending task.
    Platform::current()->currentThread()->scheduler()->postLoadingTask(FROM_HERE, m_executeScriptsTaskFactory.cancelAndCreate());
}

DEFINE_TRACE(ScriptRunner)
{
#if ENABLE(OILPAN)
    visitor->trace(m_document);
    visitor->trace(m_scriptsToExecuteInOrder);
    visitor->trace(m_scriptsToExecuteSoon);
    visitor->trace(m_pendingAsyncScripts);
#endif
}

}
