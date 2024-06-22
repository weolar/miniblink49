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
#include "core/html/parser/HTMLScriptRunner.h"

#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "bindings/core/v8/ModuleRecord.h"
#include "bindings/core/v8/Modulator.h"
#include "core/dom/Element.h"
#include "core/events/Event.h"
#include "core/dom/IgnoreDestructiveWriteCountIncrementer.h"
#include "core/dom/Microtask.h"
#include "core/dom/ScriptLoader.h"
#include "core/fetch/ScriptResource.h"
#include "core/frame/LocalFrame.h"
#include "core/html/parser/HTMLInputStream.h"
#include "core/html/parser/HTMLScriptRunnerHost.h"
#include "core/html/parser/NestingLevelIncrementer.h"
#include "platform/NotImplemented.h"
#include "public/platform/Platform.h"

namespace blink {

using namespace HTMLNames;

HTMLScriptRunner::HTMLScriptRunner(Document* document, HTMLScriptRunnerHost* host)
    : m_document(document)
    , m_host(host)
    , m_parserBlockingScript(PendingScript::create(nullptr, nullptr))
    , m_isRunningScripts(false)
    , m_scriptNestingLevel(0)
    , m_hasScriptsWaitingForResources(false)
    , m_parserBlockingScriptAlreadyLoaded(false)
{
    ASSERT(m_host);
}

HTMLScriptRunner::~HTMLScriptRunner()
{
#if ENABLE(OILPAN)
    // If the document is destructed without having explicitly
    // detached the parser (and this script runner object), perform
    // detach steps now. This will happen if the Document, the parser
    // and this script runner object are swept out in the same GC.
    detach();
#else
    // Verify that detach() has been called.
    ASSERT(!m_document);
#endif
}

void HTMLScriptRunner::detach()
{
    if (!m_document)
        return;

//     m_parserBlockingScript->stopWatchingForLoad(this);
//     m_parserBlockingScript->releaseElementAndClear();
    m_parserBlockingScript->dispose(this);

    while (!m_scriptsToExecuteAfterParsing.isEmpty()) {
        PendingScript* pendingScript = m_scriptsToExecuteAfterParsing.takeFirst();
        pendingScript->dispose(this);
//         pendingScript->stopWatchingForLoad(this);
//         pendingScript->releaseElementAndClear();
    }
    m_document = nullptr;
}

static KURL documentURLForScriptExecution(Document* document)
{
    if (!document)
        return KURL();

    if (!document->frame()) {
        if (document->importsController())
            return document->url();
        return KURL();
    }

    // Use the URL of the currently active document for this frame.
    return document->frame()->document()->url();
}

inline PassRefPtrWillBeRawPtr<Event> createScriptLoadEvent()
{
    return Event::create(EventTypeNames::load);
}

bool HTMLScriptRunner::isPendingScriptReady(const PendingScript& script)
{
    m_hasScriptsWaitingForResources = !m_document->isScriptExecutionReady();
    if (m_hasScriptsWaitingForResources)
        return false;
    return script.isReady();
}

void HTMLScriptRunner::executeParsingBlockingScript()
{
    ASSERT(m_document);
    ASSERT(!isExecutingScript());
    ASSERT(m_document->isScriptExecutionReady());
    ASSERT(isPendingScriptReady(*m_parserBlockingScript));

    InsertionPointRecord insertionPointRecord(m_host->inputStream());
    executePendingScriptAndDispatchEvent(m_parserBlockingScript.get(), PendingScript::ParsingBlocking);
}

void HTMLScriptRunner::executePendingModuleScript(PendingScript* pendingScript, PendingScript::Type pendingScriptType)
{
    // Stop watching loads before executeScript to prevent recursion if the script reloads itself.
    pendingScript->stopWatchingForLoad(this);

    if (!isExecutingScript()) {
        Microtask::performCheckpoint(V8PerIsolateData::mainThreadIsolate());
        if (pendingScriptType == PendingScript::ParsingBlocking) {
            m_hasScriptsWaitingForResources = !m_document->isScriptExecutionReady();
            // The parser cannot be unblocked as a microtask requested another resource
            if (m_hasScriptsWaitingForResources)
                return;
        }
    }

    ModuleRecord* moduleRecord = pendingScript->getModuleRecord();
    if (!moduleRecord)
        return;

    NestingLevelIncrementer nestingLevelIncrementer(m_scriptNestingLevel);
    IgnoreDestructiveWriteCountIncrementer ignoreDestructiveWriteCountIncrementer(m_document);

    ASSERT(isExecutingScript());
    Document* contextDocument = m_document->contextDocument().get();
    if (!contextDocument)
        return;

    LocalFrame* frame = contextDocument->frame();
    ScriptState* scriptState = ScriptState::forMainWorld(frame);
    if (!scriptState)
        return;

    v8::Isolate::Scope isoldateScope(scriptState->isolate());
    v8::HandleScope handleScope(scriptState->isolate());
    moduleRecord->executeModuleScript(scriptState->isolate(), scriptState->context());
}

void HTMLScriptRunner::executePendingScriptAndDispatchEvent(PendingScript* pendingScript, PendingScript::Type pendingScriptType)
{
    bool errorOccurred = false;
    double loadFinishTime = pendingScript->resource() && pendingScript->resource()->url().protocolIsInHTTPFamily() ? pendingScript->resource()->loadFinishTime() : 0;
    ScriptSourceCode sourceCode = pendingScript->getSource(documentURLForScriptExecution(m_document), errorOccurred);

    // Stop watching loads before executeScript to prevent recursion if the script reloads itself.
    pendingScript->stopWatchingForLoad(this);

    if (!isExecutingScript()) {
        Microtask::performCheckpoint(V8PerIsolateData::mainThreadIsolate());
        if (pendingScriptType == PendingScript::ParsingBlocking) {
            m_hasScriptsWaitingForResources = !m_document->isScriptExecutionReady();
            // The parser cannot be unblocked as a microtask requested another resource
            if (m_hasScriptsWaitingForResources)
                return;
        }
    }

    // Clear the pending script before possible rentrancy from executeScript()
    //RefPtrWillBeRawPtr<Element> element = pendingScript.releaseElementAndClear();
    Element* element = pendingScript->element();
    pendingScript->dispose(this);

    double compilationFinishTime = 0;
    if (ScriptLoader* scriptLoader = toScriptLoaderIfPossible(element)) {
        NestingLevelIncrementer nestingLevelIncrementer(m_scriptNestingLevel);
        IgnoreDestructiveWriteCountIncrementer ignoreDestructiveWriteCountIncrementer(m_document);
        if (errorOccurred)
            scriptLoader->dispatchErrorEvent();
        else {
            ASSERT(isExecutingScript());
            if (!scriptLoader->executeScript(sourceCode, &compilationFinishTime)) {
                scriptLoader->dispatchErrorEvent();
            } else {
                element->dispatchEvent(createScriptLoadEvent());
            }
        }
    }
    // The exact value doesn't matter; valid time stamps are much bigger than this value.
    const double epsilon = 1;
    if (pendingScriptType == PendingScript::ParsingBlocking && !m_parserBlockingScriptAlreadyLoaded && compilationFinishTime > epsilon && loadFinishTime > epsilon) {
        Platform::current()->histogramCustomCounts("WebCore.Scripts.ParsingBlocking.TimeBetweenLoadedAndCompiled", (compilationFinishTime - loadFinishTime) * 1000, 0, 10000, 50);
    }

    ASSERT(!isExecutingScript());
}

void HTMLScriptRunner::stopWatchingResourceForLoad(Resource* resource)
{
    if (m_parserBlockingScript->resource() == resource) {
//         m_parserBlockingScript->stopWatchingForLoad(this);
//         m_parserBlockingScript->releaseElementAndClear();
        m_parserBlockingScript->dispose(this);
        return;
    }

    WillBeHeapDeque<Member<PendingScript>>::iterator it = m_scriptsToExecuteAfterParsing.begin();
    for (; it != m_scriptsToExecuteAfterParsing.end(); ++it) {
        PendingScript* script = *it;
        if (script->resource() == resource) {
//             script.stopWatchingForLoad(this);
//             script.releaseElementAndClear();
            script->dispose(this);
            return;
        }
    }
}

void HTMLScriptRunner::notifyFinished(Resource* cachedResource)
{
    if (m_document) {
        ScriptState* scriptState = ScriptState::forMainWorld(m_document->frame());
        if (scriptState && scriptState->modulator())
            scriptState->modulator()->setScriptRunner(this);
    }

    // Handle cancellations of parser-blocking script loads without
    // notifying the host (i.e., parser) if these were initiated by nested
    // document.write()s. The cancellation may have been triggered by
    // script execution to signal an abrupt stop (e.g., window.close().)
    //
    // The parser is unprepared to be told, and doesn't need to be.
    if (isExecutingScript() && cachedResource->wasCanceled()) {
        stopWatchingResourceForLoad(cachedResource);
        return;
    }
    m_host->notifyScriptLoaded(cachedResource);
}

// Implements the steps for 'An end tag whose tag name is "script"'
// http://whatwg.org/html#scriptEndTag
// Script handling lives outside the tree builder to keep each class simple.
void HTMLScriptRunner::execute(PassRefPtrWillBeRawPtr<Element> scriptElement, const TextPosition& scriptStartPosition)
{
    ASSERT(scriptElement);
    // FIXME: If scripting is disabled, always just return.

    bool hadPreloadScanner = m_host->hasPreloadScanner();

    // Try to execute the script given to us.
    runScript(scriptElement.get(), scriptStartPosition);

    if (hasParserBlockingScript()) {
        if (isExecutingScript())
            return; // Unwind to the outermost HTMLScriptRunner::execute before continuing parsing.
        // If preload scanner got created, it is missing the source after the current insertion point. Append it and scan.
        if (!hadPreloadScanner && m_host->hasPreloadScanner())
            m_host->appendCurrentInputStreamToPreloadScannerAndScan();
        executeParsingBlockingScripts();
    }
}

bool HTMLScriptRunner::hasParserBlockingScript() const
{
    return !!m_parserBlockingScript->element();
}

void HTMLScriptRunner::executeParsingBlockingScripts()
{
    while (hasParserBlockingScript() && isPendingScriptReady(*m_parserBlockingScript))
        executeParsingBlockingScript();
}

void HTMLScriptRunner::executeScriptsWaitingForLoad(Resource* resource)
{
    ASSERT(!isExecutingScript());
    ASSERT(hasParserBlockingScript());
    ASSERT_UNUSED(resource, m_parserBlockingScript->resource() == resource);
    ASSERT(m_parserBlockingScript->isReady());
    executeParsingBlockingScripts();
}

void HTMLScriptRunner::executeScriptsWaitingForResources()
{
    ASSERT(m_document);
    // Callers should check hasScriptsWaitingForResources() before calling
    // to prevent parser or script re-entry during </style> parsing.
    ASSERT(hasScriptsWaitingForResources());
    ASSERT(!isExecutingScript());
    ASSERT(m_document->isScriptExecutionReady());
    executeParsingBlockingScripts();
}

// 之所以要拷贝来去，就是因为m_scriptsToExecuteAfterParsing这东西存的不是指针
// static void copyPendingScriptIfNeeded(WillBeHeapDeque<Member<PendingScript>>* scripts, const PendingScript& pendingScript)
// {
//     int find = 0;
//     for (WillBeHeapDeque<PendingScript>::iterator it = scripts->begin(); it != scripts->end(); ++it) {
//         PendingScript& script = *it;
//         find += script.copyIfNeeded(pendingScript) ? 1 : 0;
//     }
// 
//     if (pendingScript.hadGetModuleDepend())
//         RELEASE_ASSERT(find == 1);
// }

bool HTMLScriptRunner::parsePendingModuleScripts()
{
    // 先确保所有module都已经下载完成，以后优化执行顺序。比如记录依赖关系，没有全部下载也可以执行完那些对未下载无依赖的js
    bool canExecuteModuleScript = true;

    WillBeHeapDeque<Member<PendingScript> >::iterator it = m_scriptsToExecuteAfterParsing.begin();
    for (; it != m_scriptsToExecuteAfterParsing.end(); ++it) {
        ASSERT(!isExecutingScript());
        ASSERT(!hasParserBlockingScript());
        //ASSERT(*it->resource() || *it->hasModuleScriptString());

        PendingScript* pendingScriptPtr = *it;
        PendingScript& pendingScript = *pendingScriptPtr;

        if (!pendingScript.isReady()) {
            if (!pendingScript.isWatchingForLoad())
                pendingScript.watchForLoad(this);
            canExecuteModuleScript = false;
            continue;
        }

#ifndef NO_MODULE
        // 这里必须拷贝一份，否则requestPendingModuleScript会析构这个pendingScript
        PendingScript& pendingScriptCopy = pendingScript;
        //pendingScript.setCopying();
        if (pendingScriptCopy.isModule() && !pendingScriptCopy.hadGetModuleDepend()) {
            Vector<String> depends = pendingScriptCopy.compileModuleAndRequestDepend(this, m_document); // 编译并且获取有哪些依赖
            for (size_t i = 0; i < depends.size(); ++i) {
                if (requestPendingModuleScript(m_document, pendingScriptCopy.getModuleRecord(), depends[i], nullptr)) { // 发出网络请求获取这些依赖
                    canExecuteModuleScript = false;
                }
            }

            if (!canExecuteModuleScript)
                it = m_scriptsToExecuteAfterParsing.begin(); // 新加入的模块脚本被放在了头部，所以要从头开始再遍历
        }
        //copyPendingScriptIfNeeded(&m_scriptsToExecuteAfterParsing, pendingScriptCopy);
#endif
    }

    return canExecuteModuleScript;
}

bool HTMLScriptRunner::executeScriptsWaitingForParsing()
{
#ifndef NO_MODULE
    if (m_isRunningScripts)
        return false;
    m_isRunningScripts = true;

    bool canExecuteModuleScript = parsePendingModuleScripts();
    if (!canExecuteModuleScript) {
        m_isRunningScripts = false;
        return false;
    }
#endif

    OutputDebugStringA("HTMLScriptRunner::executeScriptsWaitingForParsing\n");

    while (!m_scriptsToExecuteAfterParsing.isEmpty()) {
        ASSERT(!isExecutingScript());
        ASSERT(!hasParserBlockingScript());
        PendingScript* it = m_scriptsToExecuteAfterParsing.first().get();
        ASSERT(it->resource() || it->hasModuleScriptString());
        if (!it->isReady()) {
            it->watchForLoad(this);
            m_isRunningScripts = false;
            return false;
        }
        PendingScript* first = m_scriptsToExecuteAfterParsing.takeFirst();
        if (first->isModule())
            executePendingModuleScript(first, PendingScript::Deferred);
        else
            executePendingScriptAndDispatchEvent(first, PendingScript::Deferred);
        // FIXME: What is this m_document check for?
        if (!m_document) {
            m_isRunningScripts = false;
            return false;
        }
    }

    m_isRunningScripts = false;
    return true;
}

// executeScriptsWaitingForParsing里和动态import里会调用本函数
bool HTMLScriptRunner::requestPendingModuleScript(Document* document, const ModuleRecord* parentModuleRecord, const String& sourceUrl, ScriptPromiseResolver* resolver)
{
    if (!m_document)
        m_document = document;

    PendingScript* pendingScript = PendingScript::create(nullptr, nullptr);
    if (!pendingScript->requesetModuleScript(document, parentModuleRecord, sourceUrl, resolver)) // 如果url是modulator里存在的，则会返回false，不需要重复请求了
        return false;

    String testurl = "requestPendingModuleScript::";
    testurl.append(pendingScript->getModuleRecord()->url().getUTF8String());
    testurl.append("\n");
    OutputDebugStringA(testurl.utf8().data());

    ASSERT(pendingScript->resource());
    pendingScript->watchForLoad(this);

    m_scriptsToExecuteAfterParsing.prepend(pendingScript);
    return true;
}

void HTMLScriptRunner::requestParsingBlockingScript(Element* element)
{
    if (!requestPendingScript(m_parserBlockingScript, element))
        return;

    ASSERT(m_parserBlockingScript->resource());

    // We only care about a load callback if resource is not already
    // in the cache. Callers will attempt to run the m_parserBlockingScript
    // if possible before returning control to the parser.
    if (!m_parserBlockingScript->isReady()) {
        if (m_document->frame()) {
            ScriptState* scriptState = ScriptState::forMainWorld(m_document->frame());
            if (scriptState->contextIsValid())
                ScriptStreamer::startStreaming(*m_parserBlockingScript, PendingScript::ParsingBlocking, m_document->frame()->settings(), scriptState);
        }

        m_parserBlockingScript->watchForLoad(this);
    }
}

void HTMLScriptRunner::requestDeferredScript(Element* element)
{
    PendingScript* pendingScript = PendingScript::create(nullptr, nullptr);
    if (!requestPendingScript(pendingScript, element))
        return;

    if (m_document->frame() && !pendingScript->isReady()) {
        ScriptState* scriptState = ScriptState::forMainWorld(m_document->frame());
        if (scriptState->contextIsValid())
            ScriptStreamer::startStreaming(*pendingScript, PendingScript::Deferred, m_document->frame()->settings(), scriptState);
    }

    ASSERT(pendingScript->resource() || pendingScript->hasModuleScriptString());
    m_scriptsToExecuteAfterParsing.append(pendingScript);
}

bool HTMLScriptRunner::requestPendingScript(PendingScript* pendingScript, Element* script) const
{
    ASSERT(!pendingScript->element());
    pendingScript->setElement(script);
    // This should correctly return 0 for empty or invalid srcValues.
    ScriptLoader* scriptLoader = toScriptLoaderIfPossible(script);
    ScriptResource* resource = scriptLoader->resource().get();
    if (!resource) {
        if (!scriptLoader->scriptContent().isEmpty()) {
            pendingScript->setModuleScriptString(scriptLoader->scriptContent());
            return true;
        }
        notImplemented(); // Dispatch error event.
        return false;
    }
    pendingScript->setScriptResource(resource);
    return true;
}

// Implements the initial steps for 'An end tag whose tag name is "script"'
// http://whatwg.org/html#scriptEndTag
void HTMLScriptRunner::runScript(Element* script, const TextPosition& scriptStartPosition)
{
    ASSERT(m_document);
    ASSERT(!hasParserBlockingScript());
    {
        ScriptLoader* scriptLoader = toScriptLoaderIfPossible(script);

        // This contains both and ASSERTION and a null check since we should not
        // be getting into the case of a null script element, but seem to be from
        // time to time. The assertion is left in to help find those cases and
        // is being tracked by <https://bugs.webkit.org/show_bug.cgi?id=60559>.
        ASSERT(scriptLoader);
        if (!scriptLoader)
            return;

        ASSERT(scriptLoader->isParserInserted());

        if (!isExecutingScript())
            Microtask::performCheckpoint(V8PerIsolateData::mainThreadIsolate());

        InsertionPointRecord insertionPointRecord(m_host->inputStream());
        NestingLevelIncrementer nestingLevelIncrementer(m_scriptNestingLevel);

        scriptLoader->prepareScript(scriptStartPosition);

        if (!scriptLoader->willBeParserExecuted())
            return;

        if (scriptLoader->willExecuteWhenDocumentFinishedParsing()) {
            requestDeferredScript(script);
        } else if (scriptLoader->readyToBeParserExecuted()) {
            if (m_scriptNestingLevel == 1) {
                m_parserBlockingScript->setElement(script);
                m_parserBlockingScript->setStartingPosition(scriptStartPosition);
            } else {
                ScriptSourceCode sourceCode(script->textContent(), documentURLForScriptExecution(m_document), scriptStartPosition);
                scriptLoader->executeScript(sourceCode);
            }
        } else {
            requestParsingBlockingScript(script);
        }
    }
}

DEFINE_TRACE(HTMLScriptRunner)
{
    visitor->trace(m_document);
    visitor->trace(m_host);
    visitor->trace(m_parserBlockingScript);
    visitor->trace(m_scriptsToExecuteAfterParsing);
}

} // namespace blink
