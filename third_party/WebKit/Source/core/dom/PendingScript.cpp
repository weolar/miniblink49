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
#include "core/dom/Element.h"
#include "core/fetch/ScriptResource.h"

namespace blink {

PendingScript::PendingScript()
    : m_watchingForLoad(false)
    , m_startingPosition(TextPosition::belowRangePosition())
{
}

PendingScript::PendingScript(Element* element, ScriptResource* resource)
    : m_watchingForLoad(false)
    , m_element(element)
{
    setScriptResource(resource);
}

PendingScript::PendingScript(const PendingScript& other)
    : ResourceOwner(other)
    , m_watchingForLoad(other.m_watchingForLoad)
    , m_element(other.m_element)
    , m_startingPosition(other.m_startingPosition)
    , m_streamer(other.m_streamer)
{
    setScriptResource(other.resource());
}

PendingScript::~PendingScript()
{
}

PendingScript& PendingScript::operator=(const PendingScript& other)
{
    if (this == &other)
        return *this;

    m_watchingForLoad = other.m_watchingForLoad;
    m_element = other.m_element;
    m_startingPosition = other.m_startingPosition;
    m_streamer = other.m_streamer;
    this->ResourceOwner<ScriptResource, ScriptResourceClient>::operator=(other);
    return *this;
}

void PendingScript::watchForLoad(ScriptResourceClient* client)
{
    ASSERT(!m_watchingForLoad);
    // addClient() will call notifyFinished() if the load is complete. Callers
    // who do not expect to be re-entered from this call should not call
    // watchForLoad for a PendingScript which isReady. We also need to set
    // m_watchingForLoad early, since addClient() can result in calling
    // notifyFinished and further stopWatchingForLoad().
    m_watchingForLoad = true;
    if (m_streamer) {
        m_streamer->addClient(client);
    } else {
        resource()->addClient(client);
    }
}

void PendingScript::stopWatchingForLoad(ScriptResourceClient* client)
{
    if (!m_watchingForLoad)
        return;
    ASSERT(resource());
    if (m_streamer) {
        m_streamer->removeClient(client);
    } else {
        resource()->removeClient(client);
    }
    m_watchingForLoad = false;
}

void PendingScript::setElement(Element* element)
{
    m_element = element;
}

PassRefPtrWillBeRawPtr<Element> PendingScript::releaseElementAndClear()
{
    setScriptResource(0);
    m_watchingForLoad = false;
    m_startingPosition = TextPosition::belowRangePosition();
    if (m_streamer)
        m_streamer->cancel();
    m_streamer.release();
    return m_element.release();
}

void PendingScript::setScriptResource(ScriptResource* resource)
{
    setResource(resource);
}

void PendingScript::notifyFinished(Resource* resource)
{
    if (m_streamer)
        m_streamer->notifyFinished(resource);
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
}

ScriptSourceCode PendingScript::getSource(const KURL& documentURL, bool& errorOccurred) const
{
    if (resource()) {
        errorOccurred = resource()->errorOccurred();
        ASSERT(resource()->isLoaded());
        if (m_streamer && !m_streamer->streamingSuppressed())
            return ScriptSourceCode(m_streamer, resource());
        return ScriptSourceCode(resource());
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

}
