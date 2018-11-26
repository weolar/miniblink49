/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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
#include "modules/storage/InspectorDOMStorageAgent.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/InspectorFrontend.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InspectorState.h"
#include "core/page/Page.h"
#include "modules/storage/Storage.h"
#include "modules/storage/StorageNamespace.h"
#include "modules/storage/StorageNamespaceController.h"
#include "platform/JSONValues.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

#if 1 // def MINIBLINK_NOT_IMPLEMENTED

namespace DOMStorageAgentState {
static const char domStorageAgentEnabled[] = "domStorageAgentEnabled";
};

static bool hadException(ExceptionState& exceptionState, ErrorString* errorString)
{
    if (!exceptionState.hadException())
        return false;

    switch (exceptionState.code()) {
    case SecurityError:
        *errorString = "Security error";
        return true;
    default:
        *errorString = "Unknown DOM storage error";
        return true;
    }
}

InspectorDOMStorageAgent::InspectorDOMStorageAgent(Page* page)
    : InspectorBaseAgent<InspectorDOMStorageAgent, InspectorFrontend::DOMStorage>("DOMStorage")
    , m_page(page)
    , m_isEnabled(false)
{
}

InspectorDOMStorageAgent::~InspectorDOMStorageAgent()
{
}

DEFINE_TRACE(InspectorDOMStorageAgent)
{
    visitor->trace(m_page);
    InspectorBaseAgent::trace(visitor);
}

void InspectorDOMStorageAgent::restore()
{
    if (m_state->getBoolean(DOMStorageAgentState::domStorageAgentEnabled))
        enable(0);
}

void InspectorDOMStorageAgent::enable(ErrorString*)
{
    if (m_isEnabled)
        return;
    m_isEnabled = true;
    m_state->setBoolean(DOMStorageAgentState::domStorageAgentEnabled, true);
    if (StorageNamespaceController* controller = StorageNamespaceController::from(m_page))
        controller->setInspectorAgent(this);
}

void InspectorDOMStorageAgent::disable(ErrorString*)
{
    if (!m_isEnabled)
        return;
    m_isEnabled = false;
    m_state->setBoolean(DOMStorageAgentState::domStorageAgentEnabled, false);
    if (StorageNamespaceController* controller = StorageNamespaceController::from(m_page))
        controller->setInspectorAgent(nullptr);
}

void InspectorDOMStorageAgent::getDOMStorageItems(ErrorString* errorString, const RefPtr<JSONObject>& storageId, RefPtr<TypeBuilder::Array<TypeBuilder::Array<String>>>& items)
{
    LocalFrame* frame;
    StorageArea* storageArea = findStorageArea(errorString, storageId, frame);
    if (!storageArea)
        return;

    RefPtr<TypeBuilder::Array<TypeBuilder::Array<String>>> storageItems = TypeBuilder::Array<TypeBuilder::Array<String>>::create();

    TrackExceptionState exceptionState;
    for (unsigned i = 0; i < storageArea->length(exceptionState, frame); ++i) {
        String name(storageArea->key(i, exceptionState, frame));
        if (hadException(exceptionState, errorString))
            return;
        String value(storageArea->getItem(name, exceptionState, frame));
        if (hadException(exceptionState, errorString))
            return;
        RefPtr<TypeBuilder::Array<String>> entry = TypeBuilder::Array<String>::create();
        entry->addItem(name);
        entry->addItem(value);
        storageItems->addItem(entry);
    }
    items = storageItems.release();
}

static String toErrorString(ExceptionState& exceptionState)
{
    if (exceptionState.hadException())
        return DOMException::getErrorName(exceptionState.code());
    return "";
}

void InspectorDOMStorageAgent::setDOMStorageItem(ErrorString* errorString, const RefPtr<JSONObject>& storageId, const String& key, const String& value)
{
    LocalFrame* frame;
    StorageArea* storageArea = findStorageArea(0, storageId, frame);
    if (!storageArea) {
        *errorString = "Storage not found";
        return;
    }

    TrackExceptionState exceptionState;
    storageArea->setItem(key, value, exceptionState, frame);
    *errorString = toErrorString(exceptionState);
}

void InspectorDOMStorageAgent::removeDOMStorageItem(ErrorString* errorString, const RefPtr<JSONObject>& storageId, const String& key)
{
    LocalFrame* frame;
    StorageArea* storageArea = findStorageArea(0, storageId, frame);
    if (!storageArea) {
        *errorString = "Storage not found";
        return;
    }

    TrackExceptionState exceptionState;
    storageArea->removeItem(key, exceptionState, frame);
    *errorString = toErrorString(exceptionState);
}

PassRefPtr<TypeBuilder::DOMStorage::StorageId> InspectorDOMStorageAgent::storageId(SecurityOrigin* securityOrigin, bool isLocalStorage)
{
    return TypeBuilder::DOMStorage::StorageId::create()
        .setSecurityOrigin(securityOrigin->toRawString())
        .setIsLocalStorage(isLocalStorage).release();
}

void InspectorDOMStorageAgent::didDispatchDOMStorageEvent(const String& key, const String& oldValue, const String& newValue, StorageType storageType, SecurityOrigin* securityOrigin)
{
    if (!frontend())
        return;

    RefPtr<TypeBuilder::DOMStorage::StorageId> id = storageId(securityOrigin, storageType == LocalStorage);

    if (key.isNull())
        frontend()->domStorageItemsCleared(id);
    else if (newValue.isNull())
        frontend()->domStorageItemRemoved(id, key);
    else if (oldValue.isNull())
        frontend()->domStorageItemAdded(id, key, newValue);
    else
        frontend()->domStorageItemUpdated(id, key, oldValue, newValue);
}

static LocalFrame* findFrameWithSecurityOrigin(LocalFrame* inspectedFrame, const String& originRawString)
{
    for (Frame* frame = inspectedFrame; frame; frame = frame->tree().traverseNext(inspectedFrame)) {
        if (!frame->isLocalFrame())
            continue;
        RefPtr<SecurityOrigin> documentOrigin = toLocalFrame(frame)->document()->securityOrigin();
        if (documentOrigin->toRawString() == originRawString)
            return toLocalFrame(frame);
    }
    return nullptr;
}

StorageArea* InspectorDOMStorageAgent::findStorageArea(ErrorString* errorString, const RefPtr<JSONObject>& storageId, LocalFrame*& targetFrame)
{
    String securityOrigin;
    bool isLocalStorage = false;
    bool success = storageId->getString("securityOrigin", &securityOrigin);
    if (success)
        success = storageId->getBoolean("isLocalStorage", &isLocalStorage);
    if (!success) {
        if (errorString)
            *errorString = "Invalid storageId format";
        return nullptr;
    }

    if (!m_page->mainFrame()->isLocalFrame())
        return nullptr;

    LocalFrame* frame = findFrameWithSecurityOrigin(m_page->deprecatedLocalMainFrame(), securityOrigin);
    if (!frame) {
        if (errorString)
            *errorString = "LocalFrame not found for the given security origin";
        return nullptr;
    }
    targetFrame = frame;

    if (isLocalStorage)
        return StorageNamespace::localStorageArea(frame->document()->securityOrigin());
    return StorageNamespaceController::from(m_page)->sessionStorage()->storageArea(frame->document()->securityOrigin());
}

#else

void InspectorDOMStorageAgent::didDispatchDOMStorageEvent(const String& key, const String& oldValue, const String& newValue, StorageType storageType, SecurityOrigin* securityOrigin)
{
    ;
}

DEFINE_TRACE(InspectorDOMStorageAgent)
{
//     visitor->trace(m_page);
//     InspectorBaseAgent::trace(visitor);
    DebugBreak();
}

#endif // MINIBLINK_NOT_IMPLEMENTED

} // namespace blink
