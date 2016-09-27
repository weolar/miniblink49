/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Matt Lilek <webkit@mattlilek.com>
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/inspector/InjectedScriptManager.h"

#include "bindings/core/v8/ScriptValue.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InjectedScriptNative.h"
#include "core/inspector/JSONParser.h"
#include "platform/JSONValues.h"
#include "public/platform/Platform.h"
#include "public/platform/WebData.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

PassOwnPtrWillBeRawPtr<InjectedScriptManager> InjectedScriptManager::createForPage()
{
    return adoptPtrWillBeNoop(new InjectedScriptManager(&InjectedScriptManager::canAccessInspectedWindow));
}

PassOwnPtrWillBeRawPtr<InjectedScriptManager> InjectedScriptManager::createForWorker()
{
    return adoptPtrWillBeNoop(new InjectedScriptManager(&InjectedScriptManager::canAccessInspectedWorkerGlobalScope));
}

InjectedScriptManager::InjectedScriptManager(InspectedStateAccessCheck accessCheck)
    : m_nextInjectedScriptId(1)
    , m_injectedScriptHost(InjectedScriptHost::create())
    , m_inspectedStateAccessCheck(accessCheck)
    , m_customObjectFormatterEnabled(false)
{
}

InjectedScriptManager::~InjectedScriptManager()
{
}

DEFINE_TRACE(InjectedScriptManager)
{
    visitor->trace(m_injectedScriptHost);
}

void InjectedScriptManager::disconnect()
{
    m_injectedScriptHost->disconnect();
    m_injectedScriptHost.clear();
}

InjectedScriptHost* InjectedScriptManager::injectedScriptHost()
{
    return m_injectedScriptHost.get();
}

InjectedScript InjectedScriptManager::injectedScriptForId(int id)
{
    IdToInjectedScriptMap::iterator it = m_idToInjectedScript.find(id);
    if (it != m_idToInjectedScript.end())
        return it->value;
    for (auto& state : m_scriptStateToId) {
        if (state.value == id)
            return injectedScriptFor(state.key.get());
    }
    return InjectedScript();
}

int InjectedScriptManager::injectedScriptIdFor(ScriptState* scriptState)
{
    ScriptStateToId::iterator it = m_scriptStateToId.find(scriptState);
    if (it != m_scriptStateToId.end())
        return it->value;
    int id = m_nextInjectedScriptId++;
    m_scriptStateToId.set(scriptState, id);
    return id;
}

InjectedScript InjectedScriptManager::injectedScriptForObjectId(const String& objectId)
{
    RefPtr<JSONValue> parsedObjectId = parseJSON(objectId);
    if (parsedObjectId && parsedObjectId->type() == JSONValue::TypeObject) {
        long injectedScriptId = 0;
        bool success = parsedObjectId->asObject()->getNumber("injectedScriptId", &injectedScriptId);
        if (success)
            return m_idToInjectedScript.get(injectedScriptId);
    }
    return InjectedScript();
}

void InjectedScriptManager::discardInjectedScripts()
{
    m_idToInjectedScript.clear();
    m_scriptStateToId.clear();
}

void InjectedScriptManager::discardInjectedScriptFor(ScriptState* scriptState)
{
    ScriptStateToId::iterator it = m_scriptStateToId.find(scriptState);
    if (it == m_scriptStateToId.end())
        return;

    m_idToInjectedScript.remove(it->value);
    m_scriptStateToId.remove(it);
}

bool InjectedScriptManager::canAccessInspectedWorkerGlobalScope(ScriptState*)
{
    return true;
}

void InjectedScriptManager::releaseObjectGroup(const String& objectGroup)
{
    Vector<int> keys;
    keys.appendRange(m_idToInjectedScript.keys().begin(), m_idToInjectedScript.keys().end());
    for (auto& key : keys) {
        IdToInjectedScriptMap::iterator s = m_idToInjectedScript.find(key);
        if (s != m_idToInjectedScript.end())
            s->value.releaseObjectGroup(objectGroup); // m_idToInjectedScript may change here.
    }
}

void InjectedScriptManager::setCustomObjectFormatterEnabled(bool enabled)
{
    m_customObjectFormatterEnabled = enabled;
    IdToInjectedScriptMap::iterator end = m_idToInjectedScript.end();
    for (IdToInjectedScriptMap::iterator it = m_idToInjectedScript.begin(); it != end; ++it) {
        if (!it->value.isEmpty())
            it->value.setCustomObjectFormatterEnabled(enabled);
    }
}

String InjectedScriptManager::injectedScriptSource()
{
    const WebData& injectedScriptSourceResource = Platform::current()->loadResource("InjectedScriptSource.js");
    return String(injectedScriptSourceResource.data(), injectedScriptSourceResource.size());
}

InjectedScript InjectedScriptManager::injectedScriptFor(ScriptState* inspectedScriptState)
{
    ScriptStateToId::iterator it = m_scriptStateToId.find(inspectedScriptState);
    if (it != m_scriptStateToId.end()) {
        IdToInjectedScriptMap::iterator it1 = m_idToInjectedScript.find(it->value);
        if (it1 != m_idToInjectedScript.end())
            return it1->value;
    }

    if (!m_inspectedStateAccessCheck(inspectedScriptState))
        return InjectedScript();

    int id = injectedScriptIdFor(inspectedScriptState);
    RefPtr<InjectedScriptNative> injectedScriptNative = adoptRef(new InjectedScriptNative(inspectedScriptState->isolate()));
    ScriptValue injectedScriptValue = createInjectedScript(injectedScriptSource(), inspectedScriptState, id, injectedScriptNative.get());
    InjectedScript result(injectedScriptValue, m_inspectedStateAccessCheck, injectedScriptNative.release());
    if (m_customObjectFormatterEnabled)
        result.setCustomObjectFormatterEnabled(m_customObjectFormatterEnabled);
    m_idToInjectedScript.set(id, result);
    return result;
}

} // namespace blink
