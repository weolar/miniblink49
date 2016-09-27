/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/inspector/InspectorDOMDebuggerAgent.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/InspectorFrontend.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/events/Event.h"
#include "core/events/EventTarget.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/inspector/EventListenerInfo.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptManager.h"
#include "core/inspector/InspectorDOMAgent.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "platform/JSONValues.h"

namespace {

enum DOMBreakpointType {
    SubtreeModified = 0,
    AttributeModified,
    NodeRemoved,
    DOMBreakpointTypesCount
};

static const char listenerEventCategoryType[] = "listener:";
static const char instrumentationEventCategoryType[] = "instrumentation:";

const uint32_t inheritableDOMBreakpointTypesMask = (1 << SubtreeModified);
const int domBreakpointDerivedTypeShift = 16;

}

namespace blink {

static const char requestAnimationFrameEventName[] = "requestAnimationFrame";
static const char cancelAnimationFrameEventName[] = "cancelAnimationFrame";
static const char animationFrameFiredEventName[] = "animationFrameFired";
static const char setInnerHTMLEventName[] = "setInnerHTML";
static const char setTimerEventName[] = "setTimer";
static const char clearTimerEventName[] = "clearTimer";
static const char timerFiredEventName[] = "timerFired";
static const char newPromiseEventName[] = "newPromise";
static const char promiseResolvedEventName[] = "promiseResolved";
static const char promiseRejectedEventName[] = "promiseRejected";
static const char scriptFirstStatementEventName[] = "scriptFirstStatement";
static const char windowCloseEventName[] = "close";
static const char webglErrorFiredEventName[] = "webglErrorFired";
static const char webglWarningFiredEventName[] = "webglWarningFired";
static const char webglErrorNameProperty[] = "webglErrorName";

namespace DOMDebuggerAgentState {
static const char eventListenerBreakpoints[] = "eventListenerBreakpoints";
static const char eventTargetAny[] = "*";
static const char pauseOnAllXHRs[] = "pauseOnAllXHRs";
static const char xhrBreakpoints[] = "xhrBreakpoints";
}

PassOwnPtrWillBeRawPtr<InspectorDOMDebuggerAgent> InspectorDOMDebuggerAgent::create(InjectedScriptManager* injectedScriptManager, InspectorDOMAgent* domAgent, InspectorDebuggerAgent* debuggerAgent)
{
    return adoptPtrWillBeNoop(new InspectorDOMDebuggerAgent(injectedScriptManager, domAgent, debuggerAgent));
}

InspectorDOMDebuggerAgent::InspectorDOMDebuggerAgent(InjectedScriptManager* injectedScriptManager, InspectorDOMAgent* domAgent, InspectorDebuggerAgent* debuggerAgent)
    : InspectorBaseAgent<InspectorDOMDebuggerAgent, InspectorFrontend::DOMDebugger>("DOMDebugger")
    , m_injectedScriptManager(injectedScriptManager)
    , m_domAgent(domAgent)
    , m_debuggerAgent(debuggerAgent)
{
    m_debuggerAgent->setListener(this);
    m_domAgent->setListener(this);
}

InspectorDOMDebuggerAgent::~InspectorDOMDebuggerAgent()
{
#if !ENABLE(OILPAN)
    ASSERT(!m_debuggerAgent);
    ASSERT(!m_domAgent);
    ASSERT(!m_instrumentingAgents->inspectorDOMDebuggerAgent());
#endif
}

DEFINE_TRACE(InspectorDOMDebuggerAgent)
{
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_domAgent);
    visitor->trace(m_debuggerAgent);
#if ENABLE(OILPAN)
    visitor->trace(m_domBreakpoints);
#endif
    InspectorBaseAgent::trace(visitor);
}

// Browser debugger agent enabled only when JS debugger is enabled.
void InspectorDOMDebuggerAgent::debuggerWasEnabled()
{
    if (m_domAgent->enabled() && m_debuggerAgent->enabled())
        m_instrumentingAgents->setInspectorDOMDebuggerAgent(this);
}

void InspectorDOMDebuggerAgent::debuggerWasDisabled()
{
    disable(nullptr);
}

void InspectorDOMDebuggerAgent::domAgentWasEnabled()
{
    if (m_domAgent->enabled() && m_debuggerAgent->enabled())
        m_instrumentingAgents->setInspectorDOMDebuggerAgent(this);
}

void InspectorDOMDebuggerAgent::domAgentWasDisabled()
{
    disable(nullptr);
}

bool InspectorDOMDebuggerAgent::checkEnabled(ErrorString* errorString)
{
    if (!m_domAgent->enabled()) {
        *errorString = "DOM domain required by DOMDebugger is not enabled";
        return false;
    }
    if (!m_debuggerAgent->enabled()) {
        *errorString = "Debugger domain required by DOMDebugger is not enabled";
        return false;
    }
    return true;
}

void InspectorDOMDebuggerAgent::disable(ErrorString*)
{
    m_instrumentingAgents->setInspectorDOMDebuggerAgent(nullptr);
    clear();
}

void InspectorDOMDebuggerAgent::discardAgent()
{
    m_debuggerAgent->setListener(nullptr);
    m_debuggerAgent = nullptr;
    m_domAgent->setListener(nullptr);
    m_domAgent = nullptr;
}

void InspectorDOMDebuggerAgent::setEventListenerBreakpoint(ErrorString* error, const String& eventName, const String* targetName)
{
    setBreakpoint(error, String(listenerEventCategoryType) + eventName, targetName);
}

void InspectorDOMDebuggerAgent::setInstrumentationBreakpoint(ErrorString* error, const String& eventName)
{
    setBreakpoint(error, String(instrumentationEventCategoryType) + eventName, 0);
}

static PassRefPtr<JSONObject> ensurePropertyObject(JSONObject* object, const String& propertyName)
{
    JSONObject::iterator it = object->find(propertyName);
    if (it != object->end())
        return it->value->asObject();

    RefPtr<JSONObject> result = JSONObject::create();
    object->setObject(propertyName, result);
    return result.release();
}

void InspectorDOMDebuggerAgent::setBreakpoint(ErrorString* error, const String& eventName, const String* targetName)
{
    if (!checkEnabled(error))
        return;
    if (eventName.isEmpty()) {
        *error = "Event name is empty";
        return;
    }

    RefPtr<JSONObject> eventListenerBreakpoints = m_state->getObject(DOMDebuggerAgentState::eventListenerBreakpoints);
    RefPtr<JSONObject> breakpointsByTarget = ensurePropertyObject(eventListenerBreakpoints.get(), eventName);
    if (!targetName || targetName->isEmpty())
        breakpointsByTarget->setBoolean(DOMDebuggerAgentState::eventTargetAny, true);
    else
        breakpointsByTarget->setBoolean(targetName->lower(), true);
    m_state->setObject(DOMDebuggerAgentState::eventListenerBreakpoints, eventListenerBreakpoints.release());
}

void InspectorDOMDebuggerAgent::removeEventListenerBreakpoint(ErrorString* error, const String& eventName, const String* targetName)
{
    removeBreakpoint(error, String(listenerEventCategoryType) + eventName, targetName);
}

void InspectorDOMDebuggerAgent::removeInstrumentationBreakpoint(ErrorString* error, const String& eventName)
{
    removeBreakpoint(error, String(instrumentationEventCategoryType) + eventName, 0);
}

void InspectorDOMDebuggerAgent::removeBreakpoint(ErrorString* error, const String& eventName, const String* targetName)
{
    if (eventName.isEmpty()) {
        *error = "Event name is empty";
        return;
    }

    RefPtr<JSONObject> eventListenerBreakpoints = m_state->getObject(DOMDebuggerAgentState::eventListenerBreakpoints);
    RefPtr<JSONObject> breakpointsByTarget = ensurePropertyObject(eventListenerBreakpoints.get(), eventName);
    if (!targetName || targetName->isEmpty())
        breakpointsByTarget->remove(DOMDebuggerAgentState::eventTargetAny);
    else
        breakpointsByTarget->remove(targetName->lower());
    m_state->setObject(DOMDebuggerAgentState::eventListenerBreakpoints, eventListenerBreakpoints.release());
}

void InspectorDOMDebuggerAgent::didInvalidateStyleAttr(Node* node)
{
    if (hasBreakpoint(node, AttributeModified)) {
        RefPtr<JSONObject> eventData = JSONObject::create();
        descriptionForDOMEvent(node, AttributeModified, false, eventData.get());
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::DOM, eventData.release());
    }
}

void InspectorDOMDebuggerAgent::didInsertDOMNode(Node* node)
{
    if (m_domBreakpoints.size()) {
        uint32_t mask = m_domBreakpoints.get(InspectorDOMAgent::innerParentNode(node));
        uint32_t inheritableTypesMask = (mask | (mask >> domBreakpointDerivedTypeShift)) & inheritableDOMBreakpointTypesMask;
        if (inheritableTypesMask)
            updateSubtreeBreakpoints(node, inheritableTypesMask, true);
    }
}

void InspectorDOMDebuggerAgent::didRemoveDOMNode(Node* node)
{
    if (m_domBreakpoints.size()) {
        // Remove subtree breakpoints.
        m_domBreakpoints.remove(node);
        WillBeHeapVector<RawPtrWillBeMember<Node> > stack(1, InspectorDOMAgent::innerFirstChild(node));
        do {
            Node* node = stack.last();
            stack.removeLast();
            if (!node)
                continue;
            m_domBreakpoints.remove(node);
            stack.append(InspectorDOMAgent::innerFirstChild(node));
            stack.append(InspectorDOMAgent::innerNextSibling(node));
        } while (!stack.isEmpty());
    }
}

static int domTypeForName(ErrorString* errorString, const String& typeString)
{
    if (typeString == "subtree-modified")
        return SubtreeModified;
    if (typeString == "attribute-modified")
        return AttributeModified;
    if (typeString == "node-removed")
        return NodeRemoved;
    *errorString = "Unknown DOM breakpoint type: " + typeString;
    return -1;
}

static String domTypeName(int type)
{
    switch (type) {
    case SubtreeModified: return "subtree-modified";
    case AttributeModified: return "attribute-modified";
    case NodeRemoved: return "node-removed";
    default: break;
    }
    return "";
}

void InspectorDOMDebuggerAgent::setDOMBreakpoint(ErrorString* errorString, int nodeId, const String& typeString)
{
    if (!checkEnabled(errorString))
        return;
    Node* node = m_domAgent->assertNode(errorString, nodeId);
    if (!node)
        return;

    int type = domTypeForName(errorString, typeString);
    if (type == -1)
        return;

    uint32_t rootBit = 1 << type;
    m_domBreakpoints.set(node, m_domBreakpoints.get(node) | rootBit);
    if (rootBit & inheritableDOMBreakpointTypesMask) {
        for (Node* child = InspectorDOMAgent::innerFirstChild(node); child; child = InspectorDOMAgent::innerNextSibling(child))
            updateSubtreeBreakpoints(child, rootBit, true);
    }
}

void InspectorDOMDebuggerAgent::removeDOMBreakpoint(ErrorString* errorString, int nodeId, const String& typeString)
{
    if (!checkEnabled(errorString))
        return;
    Node* node = m_domAgent->assertNode(errorString, nodeId);
    if (!node)
        return;
    int type = domTypeForName(errorString, typeString);
    if (type == -1)
        return;

    uint32_t rootBit = 1 << type;
    uint32_t mask = m_domBreakpoints.get(node) & ~rootBit;
    if (mask)
        m_domBreakpoints.set(node, mask);
    else
        m_domBreakpoints.remove(node);

    if ((rootBit & inheritableDOMBreakpointTypesMask) && !(mask & (rootBit << domBreakpointDerivedTypeShift))) {
        for (Node* child = InspectorDOMAgent::innerFirstChild(node); child; child = InspectorDOMAgent::innerNextSibling(child))
            updateSubtreeBreakpoints(child, rootBit, false);
    }
}

void InspectorDOMDebuggerAgent::getEventListeners(ErrorString* errorString, const String& objectId, RefPtr<TypeBuilder::Array<TypeBuilder::DOMDebugger::EventListener>>& listenersArray)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected frame has gone";
        return;
    }
    EventTarget* target = injectedScript.eventTargetForObjectId(objectId);
    if (!target) {
        *errorString = "No event target with passed objectId";
        return;
    }

    listenersArray = TypeBuilder::Array<TypeBuilder::DOMDebugger::EventListener>::create();
    Vector<EventListenerInfo> eventInformation;
    EventListenerInfo::getEventListeners(target, eventInformation, false);
    if (eventInformation.isEmpty())
        return;

    String objectGroup = injectedScript.objectIdToObjectGroupName(objectId);
    RegisteredEventListenerIterator iterator(eventInformation);
    while (const RegisteredEventListener* listener = iterator.nextRegisteredEventListener()) {
        const EventListenerInfo& info = iterator.currentEventListenerInfo();
        RefPtr<TypeBuilder::DOMDebugger::EventListener> listenerObject = buildObjectForEventListener(*listener, info.eventType, info.eventTarget, objectGroup);
        if (listenerObject)
            listenersArray->addItem(listenerObject);
    }
}

PassRefPtr<TypeBuilder::DOMDebugger::EventListener> InspectorDOMDebuggerAgent::buildObjectForEventListener(const RegisteredEventListener& registeredEventListener, const AtomicString& eventType, EventTarget* target, const String& objectGroupId)
{
    RefPtr<EventListener> eventListener = registeredEventListener.listener;
    String scriptId;
    int lineNumber;
    int columnNumber;
    ExecutionContext* context = target->executionContext();
    if (!context)
        return nullptr;
    if (!eventListenerHandlerLocation(context, eventListener.get(), scriptId, lineNumber, columnNumber))
        return nullptr;

    RefPtr<TypeBuilder::Debugger::Location> location = TypeBuilder::Debugger::Location::create()
        .setScriptId(scriptId)
        .setLineNumber(lineNumber);
    location->setColumnNumber(columnNumber);
    RefPtr<TypeBuilder::DOMDebugger::EventListener> value = TypeBuilder::DOMDebugger::EventListener::create()
        .setType(eventType)
        .setUseCapture(registeredEventListener.useCapture)
        .setLocation(location);
    if (!objectGroupId.isEmpty())
        value->setHandler(eventHandlerObject(context, eventListener.get(), m_injectedScriptManager, &objectGroupId));
    return value.release();
}

void InspectorDOMDebuggerAgent::willInsertDOMNode(Node* parent)
{
    if (hasBreakpoint(parent, SubtreeModified)) {
        RefPtr<JSONObject> eventData = JSONObject::create();
        descriptionForDOMEvent(parent, SubtreeModified, true, eventData.get());
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::DOM, eventData.release());
    }
}

void InspectorDOMDebuggerAgent::willRemoveDOMNode(Node* node)
{
    Node* parentNode = InspectorDOMAgent::innerParentNode(node);
    if (hasBreakpoint(node, NodeRemoved)) {
        RefPtr<JSONObject> eventData = JSONObject::create();
        descriptionForDOMEvent(node, NodeRemoved, false, eventData.get());
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::DOM, eventData.release());
    } else if (parentNode && hasBreakpoint(parentNode, SubtreeModified)) {
        RefPtr<JSONObject> eventData = JSONObject::create();
        descriptionForDOMEvent(node, SubtreeModified, false, eventData.get());
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::DOM, eventData.release());
    }
    didRemoveDOMNode(node);
}

void InspectorDOMDebuggerAgent::willModifyDOMAttr(Element* element, const AtomicString&, const AtomicString&)
{
    if (hasBreakpoint(element, AttributeModified)) {
        RefPtr<JSONObject> eventData = JSONObject::create();
        descriptionForDOMEvent(element, AttributeModified, false, eventData.get());
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::DOM, eventData.release());
    }
}

void InspectorDOMDebuggerAgent::willSetInnerHTML()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(setInnerHTMLEventName, 0), true);
}

void InspectorDOMDebuggerAgent::descriptionForDOMEvent(Node* target, int breakpointType, bool insertion, JSONObject* description)
{
    ASSERT(hasBreakpoint(target, breakpointType));

    Node* breakpointOwner = target;
    if ((1 << breakpointType) & inheritableDOMBreakpointTypesMask) {
        // For inheritable breakpoint types, target node isn't always the same as the node that owns a breakpoint.
        // Target node may be unknown to frontend, so we need to push it first.
        RefPtr<TypeBuilder::Runtime::RemoteObject> targetNodeObject = m_domAgent->resolveNode(target, InspectorDebuggerAgent::backtraceObjectGroup);
        description->setValue("targetNode", targetNodeObject);

        // Find breakpoint owner node.
        if (!insertion)
            breakpointOwner = InspectorDOMAgent::innerParentNode(target);
        ASSERT(breakpointOwner);
        while (!(m_domBreakpoints.get(breakpointOwner) & (1 << breakpointType))) {
            Node* parentNode = InspectorDOMAgent::innerParentNode(breakpointOwner);
            if (!parentNode)
                break;
            breakpointOwner = parentNode;
        }

        if (breakpointType == SubtreeModified)
            description->setBoolean("insertion", insertion);
    }

    int breakpointOwnerNodeId = m_domAgent->boundNodeId(breakpointOwner);
    ASSERT(breakpointOwnerNodeId);
    description->setNumber("nodeId", breakpointOwnerNodeId);
    description->setString("type", domTypeName(breakpointType));
}

bool InspectorDOMDebuggerAgent::hasBreakpoint(Node* node, int type)
{
    uint32_t rootBit = 1 << type;
    uint32_t derivedBit = rootBit << domBreakpointDerivedTypeShift;
    return m_domBreakpoints.get(node) & (rootBit | derivedBit);
}

void InspectorDOMDebuggerAgent::updateSubtreeBreakpoints(Node* node, uint32_t rootMask, bool set)
{
    uint32_t oldMask = m_domBreakpoints.get(node);
    uint32_t derivedMask = rootMask << domBreakpointDerivedTypeShift;
    uint32_t newMask = set ? oldMask | derivedMask : oldMask & ~derivedMask;
    if (newMask)
        m_domBreakpoints.set(node, newMask);
    else
        m_domBreakpoints.remove(node);

    uint32_t newRootMask = rootMask & ~newMask;
    if (!newRootMask)
        return;

    for (Node* child = InspectorDOMAgent::innerFirstChild(node); child; child = InspectorDOMAgent::innerNextSibling(child))
        updateSubtreeBreakpoints(child, newRootMask, set);
}

void InspectorDOMDebuggerAgent::pauseOnNativeEventIfNeeded(PassRefPtr<JSONObject> eventData, bool synchronous)
{
    if (!eventData)
        return;
    if (synchronous)
        m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::EventListener, eventData);
    else
        m_debuggerAgent->schedulePauseOnNextStatement(InspectorFrontend::Debugger::Reason::EventListener, eventData);
}

PassRefPtr<JSONObject> InspectorDOMDebuggerAgent::preparePauseOnNativeEventData(const String& eventName, const String* targetName)
{
    String fullEventName = (targetName ? listenerEventCategoryType : instrumentationEventCategoryType) + eventName;
    RefPtr<JSONObject> eventListenerBreakpoints = m_state->getObject(DOMDebuggerAgentState::eventListenerBreakpoints);
    JSONObject::iterator it = eventListenerBreakpoints->find(fullEventName);
    if (it == eventListenerBreakpoints->end())
        return nullptr;
    bool match = false;
    RefPtr<JSONObject> breakpointsByTarget = it->value->asObject();
    breakpointsByTarget->getBoolean(DOMDebuggerAgentState::eventTargetAny, &match);
    if (!match && targetName)
        breakpointsByTarget->getBoolean(targetName->lower(), &match);
    if (!match)
        return nullptr;

    RefPtr<JSONObject> eventData = JSONObject::create();
    eventData->setString("eventName", fullEventName);
    if (targetName)
        eventData->setString("targetName", *targetName);
    return eventData.release();
}

void InspectorDOMDebuggerAgent::didInstallTimer(ExecutionContext*, int, int, bool)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(setTimerEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didRemoveTimer(ExecutionContext*, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(clearTimerEventName, 0), true);
}

void InspectorDOMDebuggerAgent::willFireTimer(ExecutionContext*, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(timerFiredEventName, 0), false);
}

bool InspectorDOMDebuggerAgent::canPauseOnPromiseEvent()
{
    RefPtr<JSONObject> eventListenerBreakpoints = m_state->getObject(DOMDebuggerAgentState::eventListenerBreakpoints);
    JSONObject::iterator end = eventListenerBreakpoints->end();
    return eventListenerBreakpoints->find(String(instrumentationEventCategoryType) + newPromiseEventName) != end
        || eventListenerBreakpoints->find(String(instrumentationEventCategoryType) + promiseResolvedEventName) != end
        || eventListenerBreakpoints->find(String(instrumentationEventCategoryType) + promiseRejectedEventName) != end;
}

void InspectorDOMDebuggerAgent::didCreatePromise()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(newPromiseEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didResolvePromise()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(promiseResolvedEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didRejectPromise()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(promiseRejectedEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didRequestAnimationFrame(ExecutionContext*, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(requestAnimationFrameEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didCancelAnimationFrame(ExecutionContext*, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(cancelAnimationFrameEventName, 0), true);
}

void InspectorDOMDebuggerAgent::willFireAnimationFrame(ExecutionContext*, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(animationFrameFiredEventName, 0), false);
}

void InspectorDOMDebuggerAgent::willHandleEvent(EventTarget* target, Event* event, EventListener*, bool)
{
    Node* node = target->toNode();
    String targetName = node ? node->nodeName() : target->interfaceName();
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(event->type(), &targetName), false);
}

void InspectorDOMDebuggerAgent::willEvaluateScript(LocalFrame*, const String& url, int)
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(scriptFirstStatementEventName, 0), false);
}

void InspectorDOMDebuggerAgent::willCloseWindow()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(windowCloseEventName, 0), true);
}

void InspectorDOMDebuggerAgent::didFireWebGLError(const String& errorName)
{
    RefPtr<JSONObject> eventData = preparePauseOnNativeEventData(webglErrorFiredEventName, 0);
    if (!eventData)
        return;
    if (!errorName.isEmpty())
        eventData->setString(webglErrorNameProperty, errorName);
    pauseOnNativeEventIfNeeded(eventData.release(), m_debuggerAgent->canBreakProgram());
}

void InspectorDOMDebuggerAgent::didFireWebGLWarning()
{
    pauseOnNativeEventIfNeeded(preparePauseOnNativeEventData(webglWarningFiredEventName, 0), m_debuggerAgent->canBreakProgram());
}

void InspectorDOMDebuggerAgent::didFireWebGLErrorOrWarning(const String& message)
{
    if (message.findIgnoringCase("error") != WTF::kNotFound)
        didFireWebGLError(String());
    else
        didFireWebGLWarning();
}

void InspectorDOMDebuggerAgent::setXHRBreakpoint(ErrorString* errorString, const String& url)
{
    if (!checkEnabled(errorString))
        return;
    if (url.isEmpty()) {
        m_state->setBoolean(DOMDebuggerAgentState::pauseOnAllXHRs, true);
        return;
    }

    RefPtr<JSONObject> xhrBreakpoints = m_state->getObject(DOMDebuggerAgentState::xhrBreakpoints);
    xhrBreakpoints->setBoolean(url, true);
    m_state->setObject(DOMDebuggerAgentState::xhrBreakpoints, xhrBreakpoints.release());
}

void InspectorDOMDebuggerAgent::removeXHRBreakpoint(ErrorString* errorString, const String& url)
{
    if (url.isEmpty()) {
        m_state->setBoolean(DOMDebuggerAgentState::pauseOnAllXHRs, false);
        return;
    }

    RefPtr<JSONObject> xhrBreakpoints = m_state->getObject(DOMDebuggerAgentState::xhrBreakpoints);
    xhrBreakpoints->remove(url);
    m_state->setObject(DOMDebuggerAgentState::xhrBreakpoints, xhrBreakpoints.release());
}

void InspectorDOMDebuggerAgent::willSendXMLHttpRequest(const String& url)
{
    String breakpointURL;
    if (m_state->getBoolean(DOMDebuggerAgentState::pauseOnAllXHRs))
        breakpointURL = "";
    else {
        RefPtr<JSONObject> xhrBreakpoints = m_state->getObject(DOMDebuggerAgentState::xhrBreakpoints);
        for (auto& breakpoint : *xhrBreakpoints) {
            if (url.contains(breakpoint.key)) {
                breakpointURL = breakpoint.key;
                break;
            }
        }
    }

    if (breakpointURL.isNull())
        return;

    RefPtr<JSONObject> eventData = JSONObject::create();
    eventData->setString("breakpointURL", breakpointURL);
    eventData->setString("url", url);
    m_debuggerAgent->breakProgram(InspectorFrontend::Debugger::Reason::XHR, eventData.release());
}

void InspectorDOMDebuggerAgent::clear()
{
    m_domBreakpoints.clear();
}

} // namespace blink
