// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/EventListenerInfo.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/Document.h"
#include "core/dom/Node.h"
#include "core/events/EventTarget.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptManager.h"

namespace blink {

void EventListenerInfo::getEventListeners(EventTarget* target, Vector<EventListenerInfo>& eventInformation, bool includeAncestors)
{
    // The Node's Ancestors including self.
    Vector<EventTarget*> ancestors;
    ancestors.append(target);
    if (includeAncestors) {
        Node* node = target->toNode();
        for (ContainerNode* ancestor = node ? node->parentOrShadowHostNode() : nullptr; ancestor; ancestor = ancestor->parentOrShadowHostNode())
            ancestors.append(ancestor);
    }

    // Nodes and their Listeners for the concerned event types (order is top to bottom)
    for (size_t i = ancestors.size(); i; --i) {
        EventTarget* ancestor = ancestors[i - 1];
        Vector<AtomicString> eventTypes = ancestor->eventTypes();
        for (size_t j = 0; j < eventTypes.size(); ++j) {
            AtomicString& type = eventTypes[j];
            const EventListenerVector& listeners = ancestor->getEventListeners(type);
            EventListenerVector filteredListeners;
            filteredListeners.reserveCapacity(listeners.size());
            for (size_t k = 0; k < listeners.size(); ++k) {
                if (listeners[k].listener->type() == EventListener::JSEventListenerType)
                    filteredListeners.append(listeners[k]);
            }
            if (!filteredListeners.isEmpty())
                eventInformation.append(EventListenerInfo(ancestor, type, filteredListeners));
        }
    }
}

const RegisteredEventListener* RegisteredEventListenerIterator::nextRegisteredEventListener()
{
    while (true) {
        const EventListenerInfo& info = m_listenersArray[m_infoIndex];
        const EventListenerVector& vector = info.eventListenerVector;

        for (; m_listenerIndex < vector.size(); ++m_listenerIndex) {
            if (m_isUseCapturePass == vector[m_listenerIndex].useCapture)
                return &vector[m_listenerIndex++];
        }

        m_listenerIndex = 0;
        if (m_isUseCapturePass) {
            if (++m_infoIndex >= m_listenersArray.size())
                m_isUseCapturePass = false;
        }

        if (!m_isUseCapturePass) {
            if (m_infoIndex == 0)
                return nullptr;
            --m_infoIndex;
        }
    }
}

const EventListenerInfo& RegisteredEventListenerIterator::currentEventListenerInfo()
{
    return m_listenersArray[m_infoIndex];
}

PassRefPtr<TypeBuilder::Runtime::RemoteObject> eventHandlerObject(ExecutionContext* context, EventListener* eventListener, InjectedScriptManager* manager, const String* objectGroupId)
{
    ScriptValue functionValue = eventListenerHandler(context, eventListener);
    if (functionValue.isEmpty() || !context->isDocument())
        return nullptr;

    LocalFrame* frame = toDocument(context)->frame();
    if (!frame)
        return nullptr;

    ScriptState* scriptState = eventListenerHandlerScriptState(frame, eventListener);
    if (scriptState) {
        InjectedScript injectedScript = manager->injectedScriptFor(scriptState);
        if (!injectedScript.isEmpty())
            return injectedScript.wrapObject(functionValue, *objectGroupId);
    }
    return nullptr;
}

} // namespace blink
