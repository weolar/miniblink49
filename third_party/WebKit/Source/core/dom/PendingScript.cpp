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
#include "core/dom/PendingScript.h"

#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/ScriptStreamer.h"
#include "bindings/core/v8/Modulator.h"
#include "core/dom/Element.h"
#include "core/fetch/ScriptResource.h"
#include "core/dom/ModuleScriptLoader.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "bindings/core/v8/ModuleRecord.h"

namespace blink {

PendingScript* PendingScript::create(Element* element, ScriptResource* resource)
{
    return new PendingScript(element, resource);
}

// PendingScript::PendingScript()
//     : m_watchingForLoad(false)
//     , m_isModule(false)
//     , m_hadGetModuleDepend(false)
//     , m_isCopying(false)
//     , m_startingPosition(TextPosition::belowRangePosition())
// {
// }

PendingScript::PendingScript(Element* element, ScriptResource* resource)
    : m_watchingForLoad(false)
    , m_element(element)
    //////////////////////////////////////////////////////////////////////////
    , m_isModule(false)
    , m_hadGetModuleDepend(false)
    , m_isCopying(false)
    //, m_client(nullptr)
    //////////////////////////////////////////////////////////////////////////
{
    if (element)
        m_isModule = equalIgnoringCase(m_element->getAttribute(HTMLNames::typeAttr), "module");
    setScriptResource(resource);
}

// PendingScript::PendingScript(const PendingScript& other)
//     : ResourceOwner(other)
//     , m_watchingForLoad(other.m_watchingForLoad)
//     //////////////////////////////////////////////////////////////////////////
//     , m_isModule(other.m_isModule)
//     , m_hadGetModuleDepend(other.m_hadGetModuleDepend)
//     , m_isCopying(other.m_isCopying)
//     , m_moduleScriptLoader(other.m_moduleScriptLoader)
//     , m_moduleRecord(other.m_moduleRecord)
//     //////////////////////////////////////////////////////////////////////////
//     , m_element(other.m_element)
//     , m_startingPosition(other.m_startingPosition)
//     , m_streamer(other.m_streamer)
// {
//     setScriptResource(other.resource());
// }

PendingScript::~PendingScript()
{
}

// PendingScript& PendingScript::operator=(const PendingScript& other)
// {
//     if (this == &other)
//         return *this;
// 
//     m_watchingForLoad = other.m_watchingForLoad;
//     m_element = other.m_element;
//     m_startingPosition = other.m_startingPosition;
//     m_streamer = other.m_streamer;
//     //////////////////////////////////////////////////////////////////////////
//     m_hadGetModuleDepend = other.m_hadGetModuleDepend;
//     m_isModule = other.m_isModule;
//     m_isCopying = other.m_isCopying;
//     m_moduleScriptLoader = other.m_moduleScriptLoader;
//     m_moduleRecord = other.m_moduleRecord;
//     //////////////////////////////////////////////////////////////////////////
//     this->ResourceOwner<ScriptResource, ScriptResourceClient>::operator=(other);
//     return *this;
// }

void PendingScript::watchForLoad(ScriptResourceClient* client)
{
    ASSERT(!m_watchingForLoad);
    //ASSERT(!m_client);
    // addClient() will call notifyFinished() if the load is complete. Callers
    // who do not expect to be re-entered from this call should not call
    // watchForLoad for a PendingScript which isReady. We also need to set
    // m_watchingForLoad early, since addClient() can result in calling
    // notifyFinished and further stopWatchingForLoad().
    m_watchingForLoad = true;
    //m_client = client;

    if (m_streamer) {
        m_streamer->addClient(client);
    } else {
        resource()->addClient(client);
    }
}

void PendingScript::stopWatchingForLoad(ScriptResourceClient* client)
{
    //ASSERT(!m_client || client == m_client);
    if (!m_watchingForLoad)
        return;
    ASSERT(resource());
    if (m_streamer) {
        m_streamer->removeClient(client);
    } else {
        resource()->removeClient(client);
    }
    m_watchingForLoad = false;
    //m_client = nullptr;
}

void PendingScript::setElement(Element* element)
{
    m_element = element;
    if (element)
        m_isModule = equalIgnoringCase(m_element->getAttribute(HTMLNames::typeAttr).string(), "module");
}

void PendingScript::dispose(ScriptResourceClient* client)
{
    stopWatchingForLoad(client);
    //ASSERT(!m_client);
    ASSERT(!m_watchingForLoad);

    setScriptResource(nullptr);
    m_startingPosition = TextPosition::belowRangePosition();
    if (m_streamer)
        m_streamer->cancel();
    m_streamer = nullptr;
    m_element = nullptr;
}

// PassRefPtrWillBeRawPtr<Element> PendingScript::releaseElementAndClear()
// {
//     setScriptResource(0);
//     m_watchingForLoad = false;
//     m_startingPosition = TextPosition::belowRangePosition();
//     if (m_streamer)
//         m_streamer->cancel();
//     m_streamer.release();
//     return m_element.release();
// }

void PendingScript::setScriptResource(ScriptResource* resource)
{
    setResource(resource);
}

void PendingScript::notifyFinished(Resource* resource)
{
    if (m_streamer)
        m_streamer->notifyFinished(resource);
    if (m_moduleScriptLoader)
        m_moduleScriptLoader->notifyFinished(resource);
}

void PendingScript::notifyAppendData(ScriptResource* resource)
{
    if (m_streamer)
        m_streamer->notifyAppendData(resource);
}

DEFINE_TRACE(PendingScript)
{
    visitor->trace(m_element);
    visitor->trace(m_streamer);
    visitor->trace(m_moduleScriptLoader);
    visitor->trace(m_moduleRecord);
    //visitor->trace(m_client);
}

ScriptSourceCode PendingScript::getSource(const KURL& documentURL, bool& errorOccurred) const
{
    if (resource()) {
        errorOccurred = resource()->errorOccurred();
        ASSERT(resource()->isLoaded());
        if (m_streamer && !m_streamer->streamingSuppressed())
            return ScriptSourceCode(m_streamer, resource());
        return ScriptSourceCode(resource());
    } else if (hasModuleScriptString()) {
        return ScriptSourceCode(m_moduleScriptString, documentURL);
    }
    errorOccurred = false;
    return ScriptSourceCode(m_element->textContent(), documentURL, startingPosition());
}

void PendingScript::setStreamer(PassRefPtrWillBeRawPtr<ScriptStreamer> streamer)
{
    ASSERT(!m_streamer);
    ASSERT(!m_watchingForLoad);
    m_streamer = streamer;
}

bool PendingScript::isReady() const
{
    if (resource() && !resource()->isLoaded())
        return false;
    if (m_streamer && !m_streamer->isFinished())
        return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////
// module

bool PendingScript::isModule() const
{
    return m_isModule;
}

bool PendingScript::hadGetModuleDepend() const
{
    return m_hadGetModuleDepend;
}

void PendingScript::setGetModuleDepend()
{
    ASSERT(m_isModule && !m_hadGetModuleDepend);
    m_hadGetModuleDepend = true;
}

// void PendingScript::setCopying()
// {
//     ASSERT(!m_isCopying);
//     m_isCopying = true;
// }
//
// bool PendingScript::copyIfNeeded(const PendingScript& other)
// {
//     bool isCopying = m_isCopying;
//     if (isCopying) {
//         this->operator=(other);
//     }
//     m_isCopying = false;
//     return isCopying;
// }

void PendingScript::setModuleScriptString(const String& str)
{
    m_moduleScriptString = str;
}

bool PendingScript::hasModuleScriptString() const
{
    return !m_moduleScriptString.isEmpty();
}

bool PendingScript::requesetModuleScript(Document* document, const ModuleRecord* parentModuleRecord, const String& sourceUrl, ScriptPromiseResolver* resolver)
{
    m_isModule = true;

    if (stripLeadingAndTrailingHTMLSpaces(sourceUrl).isEmpty())
        return false;

    KURL url = document->completeURL(sourceUrl);
    if (parentModuleRecord)
        url = KURL(parentModuleRecord->url(), sourceUrl);

    m_moduleScriptLoader = ModuleScriptLoader::create(document, url, resolver ? FetchRequest::NoDefer : FetchRequest::LazyLoad);
    if (!m_moduleScriptLoader)
        return false;

    ScriptState* scriptState = ScriptState::forMainWorld(document->frame());
    v8::Isolate* isolate = scriptState->isolate();
    v8::Isolate::Scope isoldateScope(isolate);
    v8::HandleScope handleScope(isolate);

    m_moduleRecord = new ModuleRecord(url, scriptState->isolate(), v8::Local<v8::Module>(), resolver);
    Modulator* modulator = Modulator::from(scriptState->context());
    modulator->add(m_moduleRecord);
    m_moduleScriptLoader->setModulator(modulator);

    setScriptResource(m_moduleScriptLoader->resource().get());
    return true;
}

Vector<String> PendingScript::compileModuleAndRequestDepend(HTMLScriptRunner* scriptRunner, Document* document)
{
    setGetModuleDepend();

    ScriptState* scriptState = ScriptState::forMainWorld(document->frame());
    v8::Isolate* isolate = scriptState->isolate();
    v8::Isolate::Scope isoldateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::EscapableHandleScope escapableHandleScope(isolate);
    ScriptState::Scope scope(scriptState);

    KURL url = document->url();
    if (resource())
        url = resource()->url();

    v8::ScriptOrigin origin(
        v8String(isolate, url.getUTF8String()),
        v8::Integer::New(isolate, startingPosition().m_line.zeroBasedInt()),
        v8::Integer::New(isolate, startingPosition().m_column.zeroBasedInt()),
        v8::Boolean::New(isolate, true),   // resource_is_shared_cross_origin
        v8::Local<v8::Integer>(),          // script id
        v8::String::Empty(isolate),        // source_map_url
        v8::Boolean::New(isolate, false),  // resource_is_opaque
        v8::False(isolate),                // is_wasm
        v8::True(isolate)                  // is_module
    );

    bool errorOccurred = false;
    double loadFinishTime = resource() && resource()->url().protocolIsInHTTPFamily() ? resource()->loadFinishTime() : 0;
    ScriptSourceCode sourceCode = getSource(/*documentURLForScriptExecution(document)*/document->url(), errorOccurred);

    Vector<String> ret;
    v8::Local<v8::String> code = v8String(isolate, sourceCode.source());
    v8::ScriptCompiler::Source source(code, origin);
    v8::MaybeLocal<v8::Module> v8module = v8::ScriptCompiler::CompileModule(isolate, &source);

    if (v8module.IsEmpty())
        return ret;

    v8::Local<v8::Module> module = v8module.ToLocalChecked();
    if (!m_moduleRecord.get()) {
        m_moduleRecord = new ModuleRecord(url, isolate, module, nullptr);
    } else {
        m_moduleRecord->setModule(isolate, module); // 里面会调用m_parentResolver->resolve
    }
    Modulator* modulator = Modulator::from(scriptState->context());
    modulator->setScriptRunner(scriptRunner);
    modulator->add(m_moduleRecord);

    int length = module->GetModuleRequestsLength();
    for (int i = 0; i < length; ++i) {
        v8::Local<v8::String> v8name = module->GetModuleRequest(i);
        ret.append(toCoreString(v8name));
    }

    return ret;
}
//////////////////////////////////////////////////////////////////////////

}
